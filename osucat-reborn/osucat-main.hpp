#pragma once

#include "message.h"
#include "osucat-addons.hpp"

using json = nlohmann::json;
using namespace std;

namespace osucat {
	class main {
	public:
		static void _CreateDUThread() {
			thread dailyUpdateThread(bind(&_DailyUpdate));
			dailyUpdateThread.detach();
		}
#pragma region 消息处理
		static void osucatMain(string message) {
			if (_stricmp(message.substr(0, 6).c_str(), "{\"data") != 0) { //忽略回执消息
				json j = json::parse(message);
				/*
				Message事件处理
				*/
				if (j["post_type"].get<string>() == "message") {
					json sj = json::parse(message)["sender"];
					osucat::Target tar;
					SenderInfo sdr;
					if (j["message_type"].get<string>() == "private") {
						tar.message_type = Target::MessageType::PRIVATE;
					}
					if (j["message_type"].get<string>() == "group") {
						tar.message_type = Target::MessageType::GROUP;
						tar.group_id = j["group_id"].get<int64_t>();
					}
					sdr.age = sj["age"].get<int>();
					sdr.nikename = sj["nickname"].get<string>();
					tar.user_id = j["user_id"].get<int64_t>();
					tar.time = j["time"].get<int64_t>();
					tar.message = j["message"].get<string>();
					char msg[32768];
					if (tar.message_type == Target::MessageType::PRIVATE) {
						sprintf_s(msg, u8"[%s] [osucat][↓]: 好友 %lld 的消息：%s", utils::unixTime2Str(tar.time).c_str(), tar.user_id, tar.message.c_str());
						cout << msg << endl;
						if (tar.message[0] == '!' || tar.message.find(u8"！") == 0) {
							string str = tar.message;
							str = tar.message[0] < 0 ? tar.message.substr(3) : tar.message.substr(1);
							monitors(str, tar, sdr);

						}
					}
					if (tar.message_type == Target::MessageType::GROUP) {
						sdr.member_role = sj["role"].get<string>();
						sprintf_s(msg, u8"[%s] [osucat][↓]: 群 %lld 的 %lld 的消息：%s", utils::unixTime2Str(tar.time).c_str(), tar.group_id, tar.user_id, tar.message.c_str());
						cout << msg << endl;
						if (tar.message[0] == '!' || tar.message.find(u8"！") == 0) {
							string str = tar.message;
							str = tar.message[0] < 0 ? tar.message.substr(3) : tar.message.substr(1);
							monitors(str, tar, sdr);
						}
						else {
							monitors(tar.message, tar, sdr, 1);
						}
					}
				}
				/*
				Request事件处理
				*/
				if (j["post_type"].get<string>() == "request") {
					Request req;
					req.message = j["comment"].get<string>();
					req.user_id = j["user_id"].get<int64_t>();
					req.flag = j["flag"].get<string>();
					req.time = j["time"].get<int64_t>();
					if (j["request_type"].get<string>() == "friend") {
						req.request_type = Request::RequestType::FRIEND;
						char msg[4096];
						sprintf_s(msg, u8"[%s] [osucat][↓]: 来自用户 %lld 的好友请求：%s\n", utils::unixTime2Str(req.time).c_str(), req.user_id, req.message.c_str());
						cout << msg << endl;
						eventActivePush(req);
					}
					if (j["request_type"].get<string>() == "group") {
						req.request_type = Request::RequestType::GROUP;
						req.GR_SubType = j["sub_type"].get<string>();
						if (j["sub_type"].get<string>() == "invite") {
							//只有受邀入群才会被处理
							char msg[4096];
							sprintf_s(msg, u8"[%s] [osucat][↓]: 来自用户 %lld 的群组邀请请求：%s\n", utils::unixTime2Str(req.time).c_str(), req.user_id, req.message.c_str());
							cout << msg << endl;
							eventActivePush(req);
						}
					}
				}
			}
		}
		static void monitors(string msg, Target tar, SenderInfo senderinfo, int monitorType = 0) {
			if (monitorType == 0) {
				string params = { 0 };
				if (cmdParse(msg, tar, senderinfo, &params)) {
					Target activepushTar;
					activepushTar.message_type = tar.message_type == Target::MessageType::PRIVATE ? Target::MessageType::PRIVATE : Target::MessageType::GROUP;
					tar.message_type == Target::MessageType::PRIVATE ? activepushTar.user_id = tar.user_id : activepushTar.group_id = tar.group_id;
					activepushTar.message = params;
					activepush(activepushTar);
				}
				return;
			}
			if (monitorType == 1) {
				string params = { 0 };
				if (funStuff(msg, tar, senderinfo, &params)) {
					Target activepushTar;
					activepushTar.message_type = tar.message_type == Target::MessageType::PRIVATE ? Target::MessageType::PRIVATE : Target::MessageType::GROUP;
					tar.message_type == Target::MessageType::PRIVATE ? activepushTar.user_id = tar.user_id : activepushTar.group_id = tar.group_id;
					activepushTar.message = params;
					activepush(activepushTar);
				}				return;
			}
		}
#pragma endregion
	private:
#pragma region 指令
#pragma region 指令解析
		static bool cmdParse(string msg, Target tar, SenderInfo senderinfo, string* params) {
			try {
				if (_stricmp(msg.substr(0, 18).c_str(), u8"猫猫调用次数") == 0) {
					Database db;
					db.Connect();
					*params = u8"猫猫从0.4版本开始，至今一共被调用了 " + to_string(db.Getcallcount()) + u8" 次。";
					return true;
				}
				if (_stricmp(msg.substr(0, 6).c_str(), "recent") == 0) {
					recent(msg.substr(6), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 2).c_str(), "pr") == 0) {
					recent(msg.substr(2), tar, params);;
					return true;
				}
				if (_stricmp(msg.substr(0, 4).c_str(), "help") == 0) {
					help(msg.substr(4), params);
					return true;
				}
				if (_stricmp(msg.substr(0, 5).c_str(), "about") == 0) {
					about(params);
					return true;
				}
				if (_stricmp(msg.substr(0, 7).c_str(), "contact") == 0) {
					contact(msg.substr(7), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 4).c_str(), "ping") == 0) {
					ping(params);
					return true;
				}
				if (_stricmp(msg.substr(0, 5).c_str(), "setid") == 0) {
					setid(msg.substr(5), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 5).c_str(), "unset") == 0) {
					unsetid(msg.substr(5), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 6).c_str(), u8"因佛") == 0) {
					info(msg.substr(6), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 4).c_str(), "info") == 0) {
					info(msg.substr(4), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 1).c_str(), "i") == 0) {
					textinfo(msg.substr(1), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 4).c_str(), "bpme") == 0) {
					return false;
				}
				if (_stricmp(msg.substr(0, 4).c_str(), "bpht") == 0) {
					bphead_tail(msg.substr(4), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 2).c_str(), "bp") == 0) {
					bp(msg.substr(2), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 5).c_str(), "score") == 0) {
					score(msg.substr(5), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 6).c_str(), "update") == 0) {
					update(msg.substr(6), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 7).c_str(), "setmode") == 0) {
					setmode(msg.substr(7), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 5).c_str(), "rctpp") == 0) {
					rctpp(msg.substr(5), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 4).c_str(), "ppvs") == 0) {
					ppvs(msg.substr(4), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 2).c_str(), "pp") == 0) {
					pp(msg.substr(2), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 4).c_str(), "with") == 0) {
					ppwith(msg.substr(4), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 9).c_str(), "badgelist") == 0) {
					badgelist(msg.substr(9), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 9).c_str(), "setbanner") == 0) {
					setbanner_v1(msg.substr(9), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 12).c_str(), "setinfopanel") == 0) {
					setinfopanel_v1(msg.substr(12), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 11).c_str(), "resetbanner") == 0) {
					resetbanner_v1(msg.substr(11), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 14).c_str(), "resetinfopanel") == 0) {
					resetinfopanel_v1(msg.substr(14), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 6).c_str(), "occost") == 0) {
					occost(msg.substr(6), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 7).c_str(), "bonuspp") == 0) {
					bonuspp(msg.substr(7), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 9).c_str(), "badgelist") == 0) {
					badgelist(msg.substr(9), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 8).c_str(), "setbadge") == 0) {
					setbadge(msg.substr(8), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 9).c_str(), "searchuid") == 0) {
					searchuid(msg.substr(9), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 6).c_str(), "switch") == 0) {
					switchfunction(msg.substr(6), tar, senderinfo, params);
					return true;
				}
				// admin commands //
				if (tar.user_id == MONO) {
					if (_stricmp(msg.substr(0, 11).c_str(), "adoptbanner") == 0) {
						adoptbanner_v1(msg.substr(11), tar, params);
						return true;
					}
					if (_stricmp(msg.substr(0, 14).c_str(), "adoptinfopanel") == 0) {
						adoptinfopanel_v1(msg.substr(14), tar, params);
						return true;
					}
					if (_stricmp(msg.substr(0, 9).c_str(), "cleartemp") == 0) {
						cleartemp(msg.substr(9), tar, params);
						return true;
					}
					if (_stricmp(msg.substr(0, 12).c_str(), "rejectbanner") == 0) {
						rejectbanner_v1(msg.substr(12), tar, params);
						return true;
					}
					if (_stricmp(msg.substr(0, 15).c_str(), "rejectinfopanel") == 0) {
						rejectinfopanel_v1(msg.substr(15), tar, params);
						return true;
					}
					if (_stricmp(msg.substr(0, 9).c_str(), "resetuser") == 0) {
						resetuser(msg.substr(9), tar, params);
						return true;
					}
					if (_stricmp(msg.substr(0, 8).c_str(), "addbadge") == 0) {
						addbadge(msg.substr(8), tar, params);
						return true;
					}
					if (_stricmp(msg.substr(0, 11).c_str(), "dailyupdate") == 0) {
						_UpdateManually(tar);
						return true;
					}
				}
#pragma region 娱乐模块
				Database db;
				db.Connect();
				if (tar.message_type == Target::MessageType::GROUP)if (db.isGroupEnable(tar.group_id, 4) == 0) return false; //拦截娱乐模块
				if (msg.find("[CQ:image") == string::npos) { //过滤图片
					if (addons::entertainment::cmdParse(msg, tar, senderinfo, params))return true;
					return false;
				}
				/*if (_stricmp(msg.substr(0, 2).c_str(), "me") == 0) {
					memyselfact(msg.substr(2), tar, senderinfo, params);
					return true;
				}*/
#pragma endregion
			}
			catch (osucat::database_exception& ex) {
				*params = u8"访问数据库时出现了一个错误，请稍后重试...";
				char reportMsg[1024];
				sprintf_s(reportMsg,
					"[%s]\n"
					u8"Mysql出现错误\n"
					u8"错误代码：%d\n"
					u8"详细信息：%s\n",
					utils::unixTime2Str(time(NULL)).c_str(),
					ex.Code(),
					ex.Info().c_str()
				);
				Target exceptionReport;
				exceptionReport.message_type = Target::MessageType::PRIVATE;
				exceptionReport.user_id = MONO;
				exceptionReport.message = reportMsg;
				activepush(exceptionReport);
				return true;
			}
			catch (osucat::NetWork_Exception& ex) {
				*params = u8"访问api时超时...请稍后重试...";
				return true;
			}
			catch (std::exception& ex) {
				*params = u8"出现了一个未知错误，请稍后重试...";
				char reportMsg[1024];
				sprintf_s(reportMsg,
					"[%s]\n"
					u8"已捕获std::exception\n"
					u8"操作者：%lld\n"
					u8"触发指令：%s\n"
					u8"详细信息：%s\n",
					utils::unixTime2Str(time(NULL)).c_str(),
					tar.user_id,
					tar.message.c_str(),
					ex.what()
				);
				Target exceptionReport;
				exceptionReport.message_type = Target::MessageType::PRIVATE;
				exceptionReport.user_id = MONO;
				exceptionReport.message = reportMsg;
				activepush(exceptionReport);
				return true;
			}
			return false;
		}
#pragma endregion
		static void help(string cmd, string* params) {
			utils::trim(cmd);
			utils::string_replace(cmd, " ", "");
			//string dir = (string)OC_ROOT_PATH + "\\", fileStr;
			if (_stricmp(cmd.substr(0, 4).c_str(), "bind") == 0) {
				*params = u8"[CQ:image,file=osucat\\help\\绑定.png]";
				return;
			}
			if (_stricmp(cmd.substr(0, 4).c_str(), "info") == 0) {
				*params = u8"[CQ:image,file=osucat\\help\\个人信息.png]";
				return;
			}
			if (_stricmp(cmd.substr(0, 5).c_str(), "score") == 0) {
				*params = u8"[CQ:image,file=osucat\\help\\成绩.png]";
				return;
			}
			if (_stricmp(cmd.substr(0, 6).c_str(), "custom") == 0) {
				*params = u8"[CQ:image,file=osucat\\help\\自定义.png]";
				return;
			}
			if (_stricmp(cmd.substr(0, 5).c_str(), "other") == 0) {
				*params = u8"[CQ:image,file=osucat\\help\\其他.png]";
				return;
			}
			*params = u8"[CQ:image,file=osucat\\help\\帮助.png]";
			return;
		}
		static void about(string* params) {
			*params = u8"[CQ:image,file=osucat\\help\\about.png]";
		}
		static void contact(string cmd, Target tar, string* params) {
			utils::trim(cmd);
			*params = u8"[CQ:at,qq=" + to_string(tar.user_id) + u8"] 你想传达的话已成功传达给麻麻了哦。";
			Target activepushTar;
			activepushTar.message_type = Target::MessageType::PRIVATE;
			activepushTar.user_id = MONO;
			activepushTar.message = u8"收到来自用户 " + to_string(tar.user_id) + u8" 的消息：" + cmd;
			activepush(activepushTar);
		}
		static void ping(string* params) {
			vector<string> rtnmsg;
			rtnmsg.push_back("pong");
			rtnmsg.push_back("paw");
			rtnmsg.push_back("meow");
			rtnmsg.push_back("[CQ:face,id=30]");
			*params = rtnmsg[utils::randomNum(0, rtnmsg.size() - 1)];
		}
		static void setid(string cmd, Target tar, string* params) {
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			if (cmd.length() > 20) {
				*params = 参数过长提示;
				return;
			}
			int64_t qq, userid;
			user_info UI = { 0 };
			Database db;
			db.Connect();
			db.addcallcount();
			userid = db.GetUserID(tar.user_id);
			if (userid != 0) {
				if (!api::GetUser(userid, mode::std, &UI) == 0) {
					*params = u8"你的账户已与osu!id为 " + UI.username + u8" 的用户绑定过了哦。如需解绑请阅读帮助信息~";
				}
				else {
					*params = 被ban账户重新绑定提示;
				}
				return;
			}
			if (api::GetUser(cmd, osu_api_v1::mode::std, &UI) == 0) {
				*params = 未从bancho检索到用户;
				return;
			}
			qq = db.GetQQ(UI.user_id);
			if (qq != 0) {
				*params =
					u8"你想要绑定的账户已与qq号为 " + to_string(qq)
					+ u8" 的用户绑定过了哦。如果你认为是他人错误绑定了你的账户，请联系麻麻~";
				return;
			}
			if (api::GetUser(cmd, osu_api_v1::mode::std, &UI) == 0) {
				*params = 未从bancho检索到用户;
				return;
			}
			char return_message[512];
			sprintf_s(return_message,
				512,
				u8"用户 %s 已成功绑定到此qq上~\n正在初始化数据，时间可能较长，请耐心等待。",
				UI.username);
			Target userreturnMsg;
			userreturnMsg.message_type = tar.message_type == Target::MessageType::PRIVATE ? Target::MessageType::PRIVATE : Target::MessageType::GROUP;
			tar.message_type == Target::MessageType::PRIVATE ? userreturnMsg.user_id = tar.user_id : userreturnMsg.group_id = tar.group_id;
			userreturnMsg.message = return_message;
			activepush(userreturnMsg);
			Target activepushTar;
			activepushTar.message_type = Target::MessageType::PRIVATE;
			activepushTar.user_id = MONO;
			activepushTar.message = u8"有1位用户绑定了他的osu!id,\nqq: " + to_string(tar.user_id)
				+ "\nosu!username: " + UI.username + "\nosu!uid: " + to_string(UI.user_id);
			activepush(activepushTar);
			time_t now = time(NULL);
			tm* tm_now = localtime(&now);
			char timeStr[30] = { 0 };
			strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_now);
			db.UserSet(UI.user_id, tar.user_id);
			db.AddUserData(&UI, timeStr);
			if (api::GetUser(cmd, osu_api_v1::mode::taiko, &UI) != 0) {
				db.AddUserData(&UI, timeStr);
			}
			if (api::GetUser(cmd, osu_api_v1::mode::ctb, &UI) != 0) {
				db.AddUserData(&UI, timeStr);
			}
			if (api::GetUser(cmd, osu_api_v1::mode::mania, &UI) != 0) {
				db.AddUserData(&UI, timeStr);
			}
			info("", tar, params);
		};
		static void unsetid(string cmd, Target tar, string* params) {
			utils::trim(cmd);
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t uid = db.GetUserID(tar.user_id);
			string msg =
				u8"你确定要解除绑定吗？解除绑定后会清除所有绑定记录以及每日数据更新记录！\n如果确定要解绑，请使用命令!"
				u8"unset yes来进行确认！\n或回复!unset+任意内容中止操作。";
			if (uid == 0) {
				*params = u8"你还没有绑定...";
				return;
			}
			if (cmd.empty()) {
				if (db.getunsetstatus(uid) == 0) {
					db.changeunsetstatus(uid, true);
					*params = msg;
				}
				else {
					db.changeunsetstatus(uid, false);
					*params = u8"解绑操作已中止。";
				}
			}
			else {
				if (cmd == "yes") {
					*params = u8"正在执行操作...";
					db.deleteuser(uid);
					DeleteFileA(("/work/v1_cover/" + to_string(uid) + "jpg").c_str());
					*params = u8"已成功解绑。";
					Target activepushTar;
					activepushTar.message_type = Target::MessageType::PRIVATE;
					activepushTar.user_id = MONO;
					activepushTar.message = u8"有1位用户解绑了他的osu!id,\nqq: " + to_string(tar.user_id)
						+ "\nosu!uid: " + to_string(uid);
					activepush(activepushTar);
				}
				else {
					db.changeunsetstatus(uid, false);
					*params = u8"解绑操作已中止。";
				}
			}
		}
		static void info(string cmd, Target tar, string* params) {
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			Database db;
			db.Connect();
			db.addcallcount();
			int temp, days = 0;
			float previousPP;
			mode gamemode;
			UserPanelData upd = { 0 };
			user_info previousUserInfo = { 0 };
			int64_t userid;
			string username = "";
			gamemode = mode::std;
			if (cmd.length() > 0) {
				if (cmd[0] == '#') {
					userid = db.GetUserID(tar.user_id);
					if (userid == 0) {
						*params = 未绑定;
						return;
					}
					gamemode = (mode)db.GetUserDefaultGameMode(userid);
					try {
						days = stoi(cmd.substr(cmd.find('#') + 1));
					}
					catch (std::exception) {
						days = 0;
					}
				}
				else if (cmd[0] == ':') {
					if (cmd.find('#') == string::npos) {
						userid = db.GetUserID(tar.user_id);
						if (userid == 0) {
							*params = 未绑定;
							return;
						}
						else {
							try {
								temp = stoi(cmd.substr(cmd.length() - 1));
								if (temp < 4 && temp > -1) {
									gamemode = (mode)temp;
								}
								else {
									gamemode = (mode)db.GetUserDefaultGameMode(userid);
								}
							}
							catch (std::exception) {
								gamemode = (mode)db.GetUserDefaultGameMode(userid);
							}
						}
					}
					else {
						userid = db.GetUserID(tar.user_id);
						if (userid == 0) {
							*params = 未绑定;
							return;
						}
						else {
							try {
								temp = stoi(cmd.substr(cmd.find(':') + 1, 1));
								days = stoi(cmd.substr(cmd.find('#') + 1));
								if (days > 3651 || days < 0) days = 3650;
								if (temp < 4 && temp > -1) {
									gamemode = (mode)temp;
								}
								else {
									gamemode = (mode)db.GetUserDefaultGameMode(userid);
								}
							}
							catch (std::exception) {
								days = 0;
								gamemode = (mode)db.GetUserDefaultGameMode(userid);
							}
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							if (cmd.find('#') != string::npos) {
								temp = stoi(cmd.substr(cmd.find(':') + 1, 1));
								days = stoi(cmd.substr(cmd.find('#') + 1));
								if (days > 3651 || days < 0) days = 3650;
							}
							else {
								temp = stoi(cmd.substr(cmd.find(':') + 1));
							}
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (mode)temp;
						}
						else {
							days = 0;
							gamemode = mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
							try {
								if (cmd.find('#') != string::npos) {
									days = stoi(cmd.substr(cmd.find('#') + 1));
									if (days > 3651 || days < 0) days = 3650;
								}
							}
							catch (std::exception) {
								days = 0;
							}
						}
						else {
							if (cmd.find('#') != string::npos) {
								username = cmd.substr(0, cmd.find('#'));
								try {
									days = stoi(cmd.substr(cmd.find('#') + 1));
									if (days > 3651 || days < 0) days = 3650;
								}
								catch (std::exception) {
									days = 0;
								}
							}
							else {
								username = cmd;
								days = 0;
							}
						}
						if (username.empty()) {
							if (api::GetUser(userid, mode::std, &upd.user_info) == 0) {
								*params = 用户已被bancho封禁;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(upd.user_info.user_id);
							}
						}
						else {
							if (username.length() > 20) {
								*params = 参数过长提示;
								return;
							}
							utils::trim(username);
							if (api::GetUser(username, mode::std, &upd.user_info) == 0) {
								*params = 未从bancho检索到用户;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(upd.user_info.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.GetUserID(tar.user_id);
				if (userid == 0) {
					*params = 未绑定;
					return;
				}
				else {
					gamemode = (mode)db.GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (api::GetUser(userid, gamemode, &upd.user_info) == 0) {
					*params = 用户已被bancho封禁;
					return;
				}
			}
			else {
				if (username.length() > 20) {
					*params = 参数过长提示;
					return;
				}
				utils::trim(username);
				if (api::GetUser(username, gamemode, &upd.user_info) == 0) {
					*params = 未从bancho检索到用户;
					return;
				}
			}
			// 剥离对pp+的依赖
			/*if (gamemode == mode::std) {
				if (db.GetUserPreviousPP(upd.user_info.user_id) != upd.user_info.pp) {
					upd.user_info.updatepplus = true;
				} else {
					upd.user_info.updatepplus = false;
					db.GetUserPreviousPPlus(upd.user_info.user_id, &upd.pplus_info);
				}
			}*/
			////
			db.GetUserPreviousPPlus(upd.user_info.user_id, &upd.pplus_info);
			////
			upd.user_info.mode = gamemode;
			previousUserInfo.mode = gamemode;
			string fileStr;
			upd.badgeid = db.getshownbadges(upd.user_info.user_id); //获取显示勋章
			if (db.GetQQ(upd.user_info.user_id) != 0) {
				if (days > 0) {
					if (db.GetUserData(upd.user_info.user_id, days, &previousUserInfo) != 0) {
						if (username.empty()) {
							*params = u8"猫猫没有查询到你在" + to_string(days) + u8"天前的数据..";
						}
						else {
							*params = u8"猫猫没有查询到ta在" + to_string(days) + u8"天前的数据..";
						}
						return;
					}
					previousUserInfo.days_before = days;
					fileStr = "osucat\\" + infoPic_v1(upd, previousUserInfo, true);
				}
				else {
					try {
						db.GetUserData(upd.user_info.user_id, 0, &previousUserInfo);
						fileStr = "osucat\\" + infoPic_v1(upd, previousUserInfo, true);
					}
					catch (osucat::database_exception) {
						fileStr = "osucat\\" + infoPic_v1(upd, upd.user_info, true);
					}
				}
			}
			else {
				fileStr = "osucat\\" + infoPic_v1(upd, upd.user_info);
			}
			*params = u8"[CQ:image,file=" + fileStr + u8"]";
			_DelayDelTmpFile(to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr);
		}
		static void textinfo(string cmd, Target tar, string* params) {
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			Database db;
			db.Connect();
			db.addcallcount();
			int temp;
			float previousPP;
			mode gamemode;
			UserPanelData upd = { 0 };
			int64_t userid;
			string username = "";
			gamemode = mode::std;
			if (cmd.length() > 0) {
				if (cmd[0] == ':') {
					userid = db.GetUserID(tar.user_id);
					if (userid == 0) {
						*params = 未绑定;
						return;
					}
					else {
						try {
							temp = stoi(cmd.substr(cmd.length() - 1));
							if (temp < 4 && temp > -1) {
								gamemode = (mode)temp;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(userid);
							}
						}
						catch (std::exception) {
							gamemode = (mode)db.GetUserDefaultGameMode(userid);
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							temp = stoi(cmd.substr(cmd.find(':') + 1));
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (mode)temp;
						}
						else {
							gamemode = mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							username = cmd;
						}
						if (username.empty()) {
							if (api::GetUser(userid, mode::std, &upd.user_info) == 0) {
								*params = 用户已被bancho封禁;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(upd.user_info.user_id);
							}
						}
						else {
							if (cmd.length() > 20) {
								*params = 参数过长提示;
								return;
							}
							utils::trim(cmd);
							if (api::GetUser(cmd, mode::std, &upd.user_info) == 0) {
								*params = 未从bancho检索到用户;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(upd.user_info.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.GetUserID(tar.user_id);
				if (userid == 0) {
					*params = 未绑定;
					return;
				}
				else {
					gamemode = (mode)db.GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (api::GetUser(userid, gamemode, &upd.user_info) == 0) {
					*params = 用户已被bancho封禁;
					return;
				}
			}
			else {
				if (username.length() > 20) {
					*params = 参数过长提示;
					return;
				}
				utils::trim(username);
				if (api::GetUser(username, gamemode, &upd.user_info) == 0) {
					*params = 未从bancho检索到用户;
					return;
				}
			}
			char message[4096];
			string modeStr;
			gamemode == mode::std
				? modeStr = "Standard"
				: gamemode == mode::taiko
				? modeStr = "Taiko"
				: gamemode == mode::ctb ? modeStr = "CtB"
				: gamemode == mode::mania ? modeStr = "Mania" : modeStr = "Unknown",
				sprintf_s(message,
					4096,
					u8"%s 的个人信息 - %s\n"
					u8"%.2fpp\n"
					u8"Global #%s\n"
					u8"%s #%s\n"
					u8"准确率：%.2f%%\n"
					u8"游戏次数：%s\n"
					u8"总命中次数：%s\n"
					u8"Ranked总分：%s\n"
					u8"总分：%s\n"
					u8"加入于%s\n"
					u8"玩了%s",
					upd.user_info.username.c_str(),
					modeStr.c_str(),
					upd.user_info.pp,
					utils::IntegerSplit(upd.user_info.global_rank).c_str(),
					utils::getCountryNameByCode(upd.user_info.country).c_str(),
					utils::IntegerSplit(upd.user_info.country_rank).c_str(),
					upd.user_info.accuracy,
					utils::IntegerSplit(upd.user_info.playcount).c_str(),
					utils::IntegerSplit(upd.user_info.n300 + upd.user_info.n100 + upd.user_info.n50).c_str(),
					utils::IntegerSplit(upd.user_info.ranked_score).c_str(),
					utils::IntegerSplit(upd.user_info.total_score).c_str(),
					utils::timeStampToDate(upd.user_info.registed_timestamp).c_str(),
					utils::Duration2StrWithoutDAY(upd.user_info.playtime).c_str());
			*params = message;
		}
		static void recent(string cmd, Target tar, string* params) {
			Database db;
			db.Connect();
			db.addcallcount();
			if (tar.message_type == Target::MessageType::GROUP) {
				if (db.isGroupEnable(tar.group_id, 3) == 0) return;
			}
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			ScorePanelData sp_data = { 0 };
			int temp;
			mode gamemode;
			gamemode = mode::std;
			string username = "";
			string beatmap;
			int64_t userid;
			char beatmap_url[512];
			if (cmd.length() > 0) {
				if (cmd[0] == ':') {
					userid = db.GetUserID(tar.user_id);
					if (userid == 0) {
						*params = 未绑定;
						return;
					}
					else {
						try {
							if (utils::isNum(cmd.substr(cmd.length() - 1))) {
								temp = stoi(cmd.substr(cmd.length() - 1));
							}
							else {
								*params = 英文模式名提示;
								return;
							}
							if (temp < 4 && temp > -1) {
								gamemode = (mode)temp;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(userid);
							}
						}
						catch (std::exception) {
							gamemode = (mode)db.GetUserDefaultGameMode(userid);
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							if (utils::isNum(cmd.substr(cmd.find(':') + 1))) {
								temp = stoi(cmd.substr(cmd.find(':') + 1));
							}
							else {
								*params = 英文模式名提示;
								return;
							}
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (mode)temp;
						}
						else {
							gamemode = mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							username = cmd;
						}
						if (username.empty()) {
							if (api::GetUser(userid, mode::std, &sp_data.user_info) == 0) {
								*params = 用户已被bancho封禁;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(sp_data.user_info.user_id);
							}
						}
						else {
							utils::trim(cmd);
							if (api::GetUser(cmd, mode::std, &sp_data.user_info) == 0) {
								*params = 未从bancho检索到用户;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(sp_data.user_info.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.GetUserID(tar.user_id);
				if (userid == 0) {
					*params = 未绑定;
					return;
				}
				else {
					gamemode = (mode)db.GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (api::GetUser(userid, gamemode, &sp_data.user_info) == 0) {
					*params = u8"没有找到记录...";
					return;
				}
				if (api::GetUserRecent(userid, gamemode, &sp_data.score_info) == 0) {
					switch (gamemode) {
					case mode::std:
						*params = u8"你最近在模式Standard上没有游玩记录~";
						return;
					case mode::taiko:
						*params = u8"你最近在模式Taiko上没有游玩记录~";
						return;
					case mode::ctb:
						*params = u8"你最近在模式Catch the Beat上没有游玩记录~";
						return;
					case mode::mania:
						*params = u8"你最近在模式Mania上没有游玩记录~";
						return;
					}
				}
			}
			else {
				if (username.length() > 20) {
					*params = 参数过长提示;
					return;
				}
				utils::trim(username);
				if (api::GetUser(username, gamemode, &sp_data.user_info) == 0) {
					*params = u8"没这个人或者TA根本不玩这个模式！";
					return;
				}
				if (api::GetUserRecent(username, gamemode, &sp_data.score_info) == 0) {
					switch (gamemode) {
					case mode::std:
						*params = u8"TA最近在模式Standard上没有游玩记录~";
						return;
					case mode::taiko:
						*params = u8"TA最近在模式Taiko上没有游玩记录~";
						return;
					case mode::ctb:
						*params = u8"TA最近在模式Catch the Beat上没有游玩记录~";
						return;
					case mode::mania:
						*params = u8"TA最近在模式Mania上没有游玩记录~";
						return;
					}
				}
			}
			sp_data.mode = gamemode;
			api::GetBeatmap(sp_data.score_info.beatmap_id, &sp_data.beatmap_info);
			sprintf_s(beatmap_url, OSU_FILE_URL "%lld", sp_data.score_info.beatmap_id);
			beatmap = NetConnection::HttpsGet(beatmap_url);
			if (beatmap.empty()) {
				*params = 获取谱面信息错误;
				return;
			}
			if (sp_data.mode == mode::std) {
				oppai pp;
				pp.read_data(beatmap);
				pp.mods((int)sp_data.score_info.mods);
				// if fc
				pp.accuracy_percent(-1);
				pp.n300((int)sp_data.score_info.n300);
				pp.n100((int)sp_data.score_info.n100);
				pp.n50((int)sp_data.score_info.n50);
				oppai_result t = pp.calc();
				sp_data.fc = (int)t.data.total_pp.value();
				// original
				pp.end((int)sp_data.score_info.n300 + (int)sp_data.score_info.n100 + (int)sp_data.score_info.n50
					+ (int)sp_data.score_info.nmiss);
				pp.nmiss((int)sp_data.score_info.nmiss);
				pp.combo((int)sp_data.score_info.combo);
				sp_data.pp_info = pp.calc();
				sp_data.pp_info.data.total_star = t.data.total_star;
				// reset
				pp.end(-1);
				pp.n300(-1);
				pp.n100(-1);
				pp.n50(-1);
				pp.nmiss(-1);
				pp.combo(-1);
				for (int i = 0; i < 5; ++i) {
					pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
					sp_data.confirmed_acc[i] = (int)(pp.calc().data.total_pp.value());
				}
				if (sp_data.pp_info.code == 0) {
					sp_data.pp_info.data.maxcombo = t.data.maxcombo;
					sp_data.pp_info.data.maxlength = t.data.length;
				}
			}
			else {
				if (sp_data.mode == mode::mania) {
					long n1, n2;
					n1 = sp_data.score_info.n50 * 50.0 + sp_data.score_info.n100 * 100.0
						+ sp_data.score_info.nkatu * 200.0
						+ (sp_data.score_info.n300 + sp_data.score_info.ngeki) * 300.0;
					n2 = 300
						* (sp_data.score_info.nmiss + sp_data.score_info.n50 + sp_data.score_info.nkatu
							+ sp_data.score_info.n100 + sp_data.score_info.n300 + sp_data.score_info.ngeki);
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == mode::taiko) {
					double n1, n2;
					n1 = ((double)sp_data.score_info.n100 + (double)sp_data.score_info.nkatu) * 0.5
						+ (double)sp_data.score_info.n300 + (double)sp_data.score_info.ngeki;
					n2 = (double)sp_data.score_info.nmiss + (double)sp_data.score_info.n100
						+ (double)sp_data.score_info.nkatu + (double)sp_data.score_info.n300
						+ (double)sp_data.score_info.ngeki;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == mode::ctb) {
					long n1, n2;
					n1 = sp_data.score_info.n50 + sp_data.score_info.n100 + sp_data.score_info.n300;
					n2 = sp_data.score_info.nkatu + sp_data.score_info.nmiss + sp_data.score_info.n50
						+ sp_data.score_info.n100 + sp_data.score_info.n300;
					sp_data.pp_info.data.maxcombo = sp_data.beatmap_info.maxcombo;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				sp_data.pp_info.data.title = sp_data.beatmap_info.title;
				sp_data.pp_info.data.artist = sp_data.beatmap_info.artist;
				sp_data.pp_info.data.creator = sp_data.beatmap_info.creator;
				sp_data.pp_info.data.total_star = sp_data.beatmap_info.stars;
				sp_data.pp_info.data.bpm = sp_data.beatmap_info.bpm;
				sp_data.pp_info.data.ar = sp_data.beatmap_info.ar;
				sp_data.pp_info.data.od = sp_data.beatmap_info.od;
				sp_data.pp_info.data.cs = sp_data.beatmap_info.cs;
				sp_data.pp_info.data.hp = sp_data.beatmap_info.hp;
				sp_data.pp_info.data.combo = sp_data.score_info.combo;
			}
			string fileStr = "osucat\\" + scorePic(sp_data);
			*params = u8"[CQ:image,file=" + fileStr + u8"]";
			db.UpdatePPRecord(tar.user_id, sp_data.score_info.beatmap_id);
			_DelayDelTmpFile(to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr);
		}
		static void bp(string cmd, Target tar, string* params) {
			Database db;
			db.Connect();
			db.addcallcount();
			if (tar.message_type == Target::MessageType::GROUP) {
				if (db.isGroupEnable(tar.group_id, 1) == 0) return;
			}
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			ScorePanelData sp_data = { 0 };
			vector<score_info> SI;
			int temp;
			mode gamemode;
			gamemode = mode::std;
			string username = "";
			string beatmap;
			int64_t userid, bpnum;
			char beatmap_url[512];
			if (cmd.length() > 0) {
				if (cmd.find('#') != string::npos) {
					if (cmd.find(':') != string::npos) {
						try {
							if (cmd.find("at,qq=") != string::npos) {
								userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
								if (userid == 0) {
									*params = 他人未绑定;
									return;
								}
							}
							else {
								username = cmd.substr(0, cmd.find('#'));
							}
						}
						catch (std::exception) {
							username = "%^%^%^!*(^&";
						}
						try {
							bpnum = stoll(cmd.substr(cmd.find('#') + 1, cmd.find(':')));
							bpnum > 100 ? bpnum = 100 : bpnum < 1 ? bpnum = 1 : bpnum = bpnum;
						}
						catch (std::exception) {
							bpnum = 1;
						}
						try {
							if (utils::isNum(cmd.substr(cmd.find(':') + 1))) {
								temp = stoi(cmd.substr(cmd.find(':') + 1));
							}
							else {
								*params = 英文模式名提示;
								return;
							}
							if (temp < 4 && temp > -1) {
								gamemode = (mode)temp;
							}
							else {
								gamemode = mode::std;
							}
						}
						catch (std::exception) {
							gamemode = mode::std;
						}
					}
					else {
						try {
							if (cmd.find("at,qq=") != string::npos) {
								userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
								if (userid == 0) {
									*params = 他人未绑定;
									return;
								}
							}
							else {
								username = cmd.substr(0, cmd.find('#'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
						}
						catch (std::exception) {
							username = "%^%^%^!*(^&";
						}
						try {
							bpnum = stoll(cmd.substr(cmd.find('#') + 1));
							bpnum > 100 ? bpnum = 100 : bpnum < 1 ? bpnum = 1 : bpnum = bpnum;
						}
						catch (std::exception) {
							bpnum = 1;
						}
						gamemode = mode::std;
					}
				}
				else if (cmd.find(':') != string::npos) {
					if (cmd[0] == ':') {
						bpnum = 1;
						userid = db.GetUserID(tar.user_id);
						if (userid == 0) {
							*params = 未绑定;
							return;
						}
						try {
							if (utils::isNum(cmd.substr(cmd.length() - 1))) {
								temp = stoi(cmd.substr(cmd.length() - 1));
							}
							else {
								*params = 英文模式名提示;
								return;
							}
							if (temp < 4 && temp > -1) {
								gamemode = (mode)temp;
							}
							else {
								gamemode = mode::std;
							}
						}
						catch (std::exception) {
							gamemode = (mode)temp;
						}
					}
					else if (cmd.find(':') != string::npos && cmd[0] != ':') {
						bpnum = 1;
						try {
							if (cmd.find("at,qq=") != string::npos) {
								userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
								if (userid == 0) {
									*params = 他人未绑定;
									return;
								}
							}
							else {
								username = cmd.substr(0, cmd.find(':'));
							}
						}
						catch (std::exception) {
							username = "%^%^%^!*(^&";
						}
						try {
							temp = stoi(cmd.substr(cmd.length() - 1));
							if (temp < 4 && temp > -1) {
								gamemode = (mode)temp;
							}
							else {
								gamemode = mode::std;
							}
						}
						catch (std::exception) {
							gamemode = (mode)temp;
						}
					}
				}
				else {
					userid = db.GetUserID(tar.user_id);
					if (userid == 0) {
						*params = 未绑定;
						return;
					}
					gamemode = (mode)db.GetUserDefaultGameMode(userid);
					if (utils::isNum(cmd)) {
						bpnum = stoll(cmd);
						bpnum > 100 ? bpnum = 100 : bpnum < 1 ? bpnum = 1 : bpnum = bpnum;
					}
					else
						bpnum = 1;
				}
			}
			else {
				userid = db.GetUserID(tar.user_id);
				if (userid == 0) {
					*params = 未绑定;
					return;
				}
				gamemode = (mode)db.GetUserDefaultGameMode(userid);
				bpnum = 1;
			}
			if (username.empty()) {
				if (api::GetUser(userid, gamemode, &sp_data.user_info) == 0) {
					*params = 用户已被bancho封禁;
					return;
				}
				if (api::GetUserBest(userid, bpnum, gamemode, SI) == 0) {
					*params = 没有查询到BP;
					return;
				}

			}
			else {
				if (username.length() > 20) {
					*params = 参数过长提示;
					return;
				}
				utils::trim(username);
				if (api::GetUser(username, gamemode, &sp_data.user_info) == 0) {
					*params = 未从bancho检索到用户;
					return;
				}
				if (api::GetUserBest(username, bpnum, gamemode, SI) == 0) {
					*params = 没有查询到他人BP;
					return;
				}
			}
			if (bpnum != SI.size()) {
				*params = 没有查询到BP;
				return;
			}
			sp_data.mode = gamemode;
			sp_data.score_info.achieved_timestamp = SI[bpnum - 1].achieved_timestamp;
			sp_data.score_info.beatmap_id = SI[bpnum - 1].beatmap_id;
			sp_data.score_info.score = SI[bpnum - 1].score;
			sp_data.score_info.combo = SI[bpnum - 1].combo;
			sp_data.score_info.n50 = SI[bpnum - 1].n50;
			sp_data.score_info.n100 = SI[bpnum - 1].n100;
			sp_data.score_info.n300 = SI[bpnum - 1].n300;
			sp_data.score_info.nkatu = SI[bpnum - 1].nkatu;
			sp_data.score_info.ngeki = SI[bpnum - 1].ngeki;
			sp_data.score_info.nmiss = SI[bpnum - 1].nmiss;
			sp_data.score_info.mods = SI[bpnum - 1].mods;
			sp_data.score_info.user_id = SI[bpnum - 1].user_id;
			sp_data.score_info.rank = SI[bpnum - 1].rank;
			sp_data.score_info.pp = SI[bpnum - 1].pp;
			sp_data.score_info.username = sp_data.user_info.username;
			api::GetBeatmap(sp_data.score_info.beatmap_id, &sp_data.beatmap_info);
			sprintf_s(beatmap_url, OSU_FILE_URL "%lld", sp_data.score_info.beatmap_id);
			beatmap = NetConnection::HttpsGet(beatmap_url);
			if (beatmap.empty()) {
				*params = 获取谱面信息错误;
				return;
			}
			if (sp_data.mode == mode::std) {
				oppai pp;
				pp.read_data(beatmap);
				pp.mods((int)sp_data.score_info.mods);
				// if fc
				pp.accuracy_percent(-1);
				pp.n300(sp_data.score_info.n300);
				pp.n100(sp_data.score_info.n100);
				pp.n50(sp_data.score_info.n50);
				oppai_result t = pp.calc();
				if (t.data.total_pp.has_value()) {
					sp_data.fc = t.data.total_pp.value();
					// original
					pp.nmiss(sp_data.score_info.nmiss);
					pp.combo(sp_data.score_info.combo);
					sp_data.pp_info = pp.calc();
					sp_data.pp_info.data.total_pp = sp_data.score_info.pp;
					sp_data.pp_info.data.total_star = t.data.total_star;
					// reset
					pp.end(-1);
					pp.n300(-1);
					pp.n100(-1);
					pp.n50(-1);
					pp.nmiss(-1);
					pp.combo(-1);
					for (int i = 0; i < 5; ++i) {
						pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
						sp_data.confirmed_acc[i] = (int)(pp.calc().data.total_pp.value());
					}
					if (sp_data.pp_info.code == 0) {
						sp_data.pp_info.data.maxcombo = t.data.maxcombo;
						sp_data.pp_info.data.maxlength = t.data.length;
					}
				}
				else {
					sp_data.fc = 0;
					sp_data.confirmed_acc[0] = 0;
					sp_data.confirmed_acc[1] = 0;
					sp_data.confirmed_acc[2] = 0;
					sp_data.confirmed_acc[3] = 0;
					sp_data.confirmed_acc[4] = 0;
					sp_data.pp_info.data.title = sp_data.beatmap_info.title;
					sp_data.pp_info.data.artist = sp_data.beatmap_info.artist;
					sp_data.pp_info.data.creator = sp_data.beatmap_info.creator;
					sp_data.pp_info.data.total_star = sp_data.beatmap_info.stars;
					sp_data.pp_info.data.bpm = sp_data.beatmap_info.bpm;
					sp_data.pp_info.data.ar = sp_data.beatmap_info.ar;
					sp_data.pp_info.data.od = sp_data.beatmap_info.od;
					sp_data.pp_info.data.cs = sp_data.beatmap_info.cs;
					sp_data.pp_info.data.hp = sp_data.beatmap_info.hp;
					sp_data.pp_info.data.combo = sp_data.score_info.combo;
					sp_data.pp_info.data.total_pp = sp_data.score_info.pp;
					sp_data.pp_info.data.maxcombo = sp_data.beatmap_info.maxcombo;
					long n1, n2;
					n1 = sp_data.score_info.n50 * 50 + sp_data.score_info.n100 * 100 + sp_data.score_info.n300 * 300;
					n2 = 300
						* (sp_data.score_info.nmiss + sp_data.score_info.n50 + sp_data.score_info.n100
							+ sp_data.score_info.n300);
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}

			}
			else {
				if (sp_data.mode == mode::mania) {
					long n1, n2;
					n1 = sp_data.score_info.n50 * 50.0 + sp_data.score_info.n100 * 100.0
						+ sp_data.score_info.nkatu * 200.0
						+ (sp_data.score_info.n300 + sp_data.score_info.ngeki) * 300.0;
					n2 = 300
						* (sp_data.score_info.nmiss + sp_data.score_info.n50 + sp_data.score_info.nkatu
							+ sp_data.score_info.n100 + sp_data.score_info.n300 + sp_data.score_info.ngeki);
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == mode::taiko) {
					double n1, n2;
					n1 = ((double)sp_data.score_info.n100 + (double)sp_data.score_info.nkatu) * 0.5
						+ (double)sp_data.score_info.n300 + (double)sp_data.score_info.ngeki;
					n2 = (double)sp_data.score_info.nmiss + (double)sp_data.score_info.n100
						+ (double)sp_data.score_info.nkatu + (double)sp_data.score_info.n300
						+ (double)sp_data.score_info.ngeki;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == mode::ctb) {
					long n1, n2;
					n1 = sp_data.score_info.n50 + sp_data.score_info.n100 + sp_data.score_info.n300;
					n2 = sp_data.score_info.nkatu + sp_data.score_info.nmiss + sp_data.score_info.n50
						+ sp_data.score_info.n100 + sp_data.score_info.n300;
					sp_data.pp_info.data.maxcombo = sp_data.beatmap_info.maxcombo;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				sp_data.pp_info.data.title = sp_data.beatmap_info.title;
				sp_data.pp_info.data.artist = sp_data.beatmap_info.artist;
				sp_data.pp_info.data.creator = sp_data.beatmap_info.creator;
				sp_data.pp_info.data.total_star = sp_data.beatmap_info.stars;
				sp_data.pp_info.data.bpm = sp_data.beatmap_info.bpm;
				sp_data.pp_info.data.ar = sp_data.beatmap_info.ar;
				sp_data.pp_info.data.od = sp_data.beatmap_info.od;
				sp_data.pp_info.data.cs = sp_data.beatmap_info.cs;
				sp_data.pp_info.data.hp = sp_data.beatmap_info.hp;
				sp_data.pp_info.data.combo = sp_data.score_info.combo;
				sp_data.pp_info.data.total_pp = sp_data.score_info.pp;
			}
			string fileStr = "osucat\\" + scorePic(sp_data);
			*params = u8"[CQ:image,file=" + fileStr + u8"]";
			db.UpdatePPRecord(tar.user_id, sp_data.score_info.beatmap_id);
			_DelayDelTmpFile(to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr);
		}
		static void score(string cmd, Target tar, string* params) {
			utils::string_replace(cmd, " ", "");
			if (!utils::isNum(cmd)) {
				*params = u8"都说啦暂时不支持文字查询啦，请输入bid来查询~";
				return;
			}
			int64_t bid = stoll(cmd);
			Database db;
			string beatmap;
			char beatmap_url[512];
			ScorePanelData sp_data = { 0 };
			db.Connect();
			db.addcallcount();
			int64_t uid = db.GetUserID(tar.user_id);
			if (uid == 0) {
				*params = 未绑定;
				return;
			}
			mode gamemode = (osu_api_v1::mode)db.GetUserDefaultGameMode(uid);
			if (api::GetUser(uid, gamemode, &sp_data.user_info) == 0) {
				*params = 用户已被bancho封禁;
				return;
			}
			if (api::GetUserScore(sp_data.user_info.user_id, gamemode, bid, &sp_data.score_info) == 0) {
				*params = 没有查询到成绩;
				return;
			}
			sp_data.mode = gamemode;
			sp_data.score_info.username = sp_data.user_info.username;
			api::GetBeatmap(sp_data.score_info.beatmap_id, &sp_data.beatmap_info);
			sprintf_s(beatmap_url, OSU_FILE_URL "%lld", sp_data.score_info.beatmap_id);
			beatmap = NetConnection::HttpsGet(beatmap_url);
			if (beatmap.empty()) {
				*params = 获取谱面信息错误;
				return;
			}
			if (sp_data.mode == mode::std) {
				oppai pp;
				pp.read_data(beatmap);
				pp.mods((int)sp_data.score_info.mods);
				// if fc
				pp.accuracy_percent(-1);
				pp.n300((int)sp_data.score_info.n300);
				pp.n100((int)sp_data.score_info.n100);
				pp.n50((int)sp_data.score_info.n50);
				oppai_result t = pp.calc();
				sp_data.fc = (int)t.data.total_pp.value();
				// original
				pp.end((int)sp_data.score_info.n300 + (int)sp_data.score_info.n100 + (int)sp_data.score_info.n50
					+ (int)sp_data.score_info.nmiss);
				pp.nmiss((int)sp_data.score_info.nmiss);
				pp.combo((int)sp_data.score_info.combo);
				sp_data.pp_info = pp.calc();
				sp_data.pp_info.data.total_star = t.data.total_star;
				// reset
				pp.end(-1);
				pp.n300(-1);
				pp.n100(-1);
				pp.n50(-1);
				pp.nmiss(-1);
				pp.combo(-1);
				for (int i = 0; i < 5; ++i) {
					pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
					sp_data.confirmed_acc[i] = (int)(pp.calc().data.total_pp.value());
				}
				if (sp_data.pp_info.code == 0) {
					sp_data.pp_info.data.maxcombo = t.data.maxcombo;
					sp_data.pp_info.data.maxlength = t.data.length;
				}
			}
			else {
				if (sp_data.mode == mode::mania) {
					long n1, n2;
					n1 = sp_data.score_info.n50 * 50.0 + sp_data.score_info.n100 * 100.0
						+ sp_data.score_info.nkatu * 200.0
						+ (sp_data.score_info.n300 + sp_data.score_info.ngeki) * 300.0;
					n2 = 300
						* (sp_data.score_info.nmiss + sp_data.score_info.n50 + sp_data.score_info.nkatu
							+ sp_data.score_info.n100 + sp_data.score_info.n300 + sp_data.score_info.ngeki);
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == mode::taiko) {
					double n1, n2;
					n1 = ((double)sp_data.score_info.n100 + (double)sp_data.score_info.nkatu) * 0.5
						+ (double)sp_data.score_info.n300 + (double)sp_data.score_info.ngeki;
					n2 = (double)sp_data.score_info.nmiss + (double)sp_data.score_info.n100
						+ (double)sp_data.score_info.nkatu + (double)sp_data.score_info.n300
						+ (double)sp_data.score_info.ngeki;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == mode::ctb) {
					long n1, n2;
					n1 = sp_data.score_info.n50 + sp_data.score_info.n100 + sp_data.score_info.n300;
					n2 = sp_data.score_info.nkatu + sp_data.score_info.nmiss + sp_data.score_info.n50
						+ sp_data.score_info.n100 + sp_data.score_info.n300;
					sp_data.pp_info.data.maxcombo = sp_data.beatmap_info.maxcombo;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				sp_data.pp_info.data.title = sp_data.beatmap_info.title;
				sp_data.pp_info.data.artist = sp_data.beatmap_info.artist;
				sp_data.pp_info.data.creator = sp_data.beatmap_info.creator;
				sp_data.pp_info.data.total_star = sp_data.beatmap_info.stars;
				sp_data.pp_info.data.bpm = sp_data.beatmap_info.bpm;
				sp_data.pp_info.data.ar = sp_data.beatmap_info.ar;
				sp_data.pp_info.data.od = sp_data.beatmap_info.od;
				sp_data.pp_info.data.cs = sp_data.beatmap_info.cs;
				sp_data.pp_info.data.hp = sp_data.beatmap_info.hp;
				sp_data.pp_info.data.combo = sp_data.score_info.combo;
			}
			string fileStr = "osucat\\" + scorePic(sp_data);
			*params = u8"[CQ:image,file=" + fileStr + u8"]";
			db.UpdatePPRecord(tar.user_id, sp_data.beatmap_info.beatmap_id);
			_DelayDelTmpFile(to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr);
		}
		static void update(string cmd, Target tar, string* params) {
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t uid = db.GetUserID(tar.user_id);
			if (uid == 0) {
				*params = 未绑定;
				return;
			}
			user_info UI;
			mode mode = (osu_api_v1::mode)db.GetUserDefaultGameMode(uid);
			int8_t returnCode = api::GetUser(uid, mode, &UI);
			if (returnCode == 0) {
				*params = 用户已被bancho封禁;
				return;
			}
			Target activepushTar;
			activepushTar.message_type = Target::MessageType::GROUP;
			activepushTar.group_id = tar.group_id;
			activepushTar.message = u8"少女祈祷中...";
			activepush(activepushTar);
			DeleteFileA(("./work/avatar/" + to_string(UI.user_id) + ".png").c_str());
			vector<long> pp_plus;
			try {
				pp_plus = NetConnection::getUserPlusData(UI.user_id);
				pplus_info pi;
				pi.acc = pp_plus[0];
				pi.flow = pp_plus[1];
				pi.jump = pp_plus[2];
				pi.pre = pp_plus[3];
				pi.spd = pp_plus[4];
				pi.sta = pp_plus[5];
				db.UpdatePPlus(UI.user_id, UI.pp, pi);
			}
			catch (std::exception) {
			}
			info("", tar, params);
		}
		static void pp(string cmd, Target tar, string* params) {
			utils::string_replace(cmd, " ", "");
			if (!utils::isNum(cmd)) {
				*params = u8"请键入正确的bid!";
				return;
			}
			string beatmap = NetConnection::HttpsGet(OSU_FILE_URL + cmd);
			if (beatmap.empty()) {
				*params = 获取谱面信息错误;
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			db.UpdatePPRecord(tar.user_id, stoll(cmd));
			oppai pp;
			vector<float> out;
			char message[5120];
			pp.read_data(beatmap);
			oppai_result t = pp.calc();
			if (t.code == -4) {
				*params = u8"暂不支持除Standard模式以外的其它模式。";
				return;
			}
			for (int i = 0; i < 5; ++i) {
				pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
				oppai_result c = pp.calc();
				out.push_back((float)(c.data.total_pp.has_value() ? c.data.total_pp.value() : 0.0));
			}
			string title, artist, version;
			if (t.data.titleUnicode.length() > 2 && t.data.titleUnicode.length() < 128) {
				title = t.data.titleUnicode;
			}
			else {
				t.data.title.length() < 255 ? title = t.data.title : title = "太长了";
			}
			if (t.data.artistUnicode.length() > 2 && t.data.titleUnicode.length() < 128) {
				artist = t.data.artistUnicode;
			}
			else {
				t.data.artist.length() < 255 ? artist = t.data.artist : artist = "太长了";
			}
			t.data.map_version.length() < 255 ? version = t.data.map_version : version = "太长了";
			sprintf_s(message,
				5120,
				u8"%s - %s[%s]\n"
				u8"mapped by %s\n"
				"ar%.1f | od%.1f | cs%.1f | hp%.1f | bpm%.2f\n"
				"Stars:%.2f* | No Mod | objects:%d\n\n"
				"100%% : %.2fpp\n"
				"99%% : %.2fpp\n"
				"98%% : %.2fpp\n"
				"97%% : %.2fpp\n"
				"95%% : %.2fpp\n\n%s",
				artist.c_str(),
				title.c_str(),
				version.c_str(),
				t.data.creator.c_str(),
				t.data.ar,
				t.data.od,
				t.data.cs,
				t.data.hp,
				t.data.bpm,
				t.data.total_star,
				t.data.n300,
				out[4],
				out[3],
				out[2],
				out[1],
				out[0],
				("https://osu.ppy.sh/b/" + cmd).c_str());
			*params = message;
		}
		static void ppwith(string cmd, Target tar, string* params) {
			if (cmd.length() > 15) {
				*params = 参数过长提示;
				return;
			}
			if (cmd.find("[CQ:") != string::npos) {
				*params = 参数过长提示;
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t bid = db.GetPPRecord(tar.user_id);
			if (bid == EOF) {
				*params = u8"你还没有查询过成绩，请先查询成绩后在来使用这条指令~";
				return;
			}
			if (!Mod(cmd).isVaild()) {
				*params = u8"参数不正确";
				return;
			}
			string beatmap = NetConnection::HttpsGet(OSU_FILE_URL + to_string(bid));
			if (beatmap.empty()) {
				*params = 获取谱面信息错误;
				return;
			}
			oppai pp;
			vector<float> out;
			char message[5120];
			pp.read_data(beatmap);
			pp.mods(Mod(cmd).GetModNumber());
			oppai_result t = pp.calc();
			for (int i = 0; i < 5; ++i) {
				pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
				out.push_back((float)(pp.calc().data.total_pp.value()));
			}
			string modStr = Mod(cmd).GetModString();
			string title, artist, version;
			if (t.data.titleUnicode.length() > 2 && t.data.titleUnicode.length() < 128) {
				title = t.data.titleUnicode;
			}
			else {
				t.data.title.length() < 255 ? title = t.data.title : title = "太长了";
			}
			if (t.data.artistUnicode.length() > 2 && t.data.titleUnicode.length() < 128) {
				artist = t.data.artistUnicode;
			}
			else {
				t.data.artist.length() < 255 ? artist = t.data.artist : artist = "太长了";
			}
			t.data.map_version.length() < 255 ? version = t.data.map_version : version = "太长了";
			sprintf_s(message,
				5120,
				u8"%s - %s[%s]\n"
				u8"mapped by %s\n"
				"[ar%.1f | od%.1f | cs%.1f | hp%.1f | bpm%.2f]\n"
				"%s | Stars:%.2f* | objects:%d\n\n"
				"100%% : %.2fpp\n"
				"99%% : %.2fpp\n"
				"98%% : %.2fpp\n"
				"97%% : %.2fpp\n"
				"95%% : %.2fpp\n\n%s",
				artist.c_str(),
				title.c_str(),
				version.c_str(),
				t.data.creator.c_str(),
				t.data.ar,
				t.data.od,
				t.data.cs,
				t.data.hp,
				t.data.bpm,
				modStr.length() == 0 ? "No Mod" : modStr.c_str(),
				t.data.total_star,
				t.data.n300,
				out[4],
				out[3],
				out[2],
				out[1],
				out[0],
				("https://osu.ppy.sh/b/" + to_string(bid)).c_str());
			*params = message;
		}
		static void rctpp(string cmd, Target tar, string* params) {
			Database db;
			db.Connect();
			db.addcallcount();
			if (tar.message_type == Target::MessageType::GROUP) {
				if (db.isGroupEnable(tar.group_id, 2) == 0) return;
			}
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			ScorePanelData sp_data = { 0 };
			int temp;
			mode gamemode;
			gamemode = mode::std;
			string username = "";
			string beatmap;
			int64_t userid;
			char beatmap_url[512];
			if (cmd.length() > 0) {
				if (cmd[0] == ':') {
					userid = db.GetUserID(tar.user_id);
					if (userid == 0) {
						*params = 未绑定;
						return;
					}
					else {
						try {
							if (utils::isNum(cmd.substr(cmd.length() - 1))) {
								temp = stoi(cmd.substr(cmd.length() - 1));
							}
							else {
								*params = 英文模式名提示;
								return;
							}
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (mode)temp;
						}
						else {
							gamemode = (mode)db.GetUserDefaultGameMode(userid);
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							if (utils::isNum(cmd.substr(cmd.find(':') + 1))) {
								temp = stoi(cmd.substr(cmd.find(':') + 1));
							}
							else {
								*params = 英文模式名提示;
								return;
							}
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (mode)temp;
						}
						else {
							gamemode = mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							username = cmd;
						}
						if (username.empty()) {
							if (api::GetUser(userid, mode::std, &sp_data.user_info) == 0) {
								*params = 用户已被bancho封禁;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(sp_data.user_info.user_id);
							}
						}
						else {
							utils::trim(cmd);
							if (api::GetUser(cmd, mode::std, &sp_data.user_info) == 0) {
								*params = 未从bancho检索到用户;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(sp_data.user_info.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.GetUserID(tar.user_id);
				if (userid == 0) {
					*params = 未绑定;
					return;
				}
				else {
					gamemode = (mode)db.GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (api::GetUser(userid, gamemode, &sp_data.user_info) == 0) {
					*params = u8"没有找到记录...";
					return;
				}
				if (api::GetUserRecent(userid, gamemode, &sp_data.score_info) == 0) {
					switch (gamemode) {
					case mode::std:
						*params = u8"你最近在模式Standard上没有游玩记录~";
						return;
					case mode::taiko:
						*params = u8"你最近在模式Taiko上没有游玩记录~";
						return;
					case mode::ctb:
						*params = u8"你最近在模式Catch the Beat上没有游玩记录~";
						return;
					case mode::mania:
						*params = u8"你最近在模式Mania上没有游玩记录~";
						return;
					}
				}
			}
			else {
				if (username.length() > 20) {
					"";
					*params = 参数过长提示;
					return;
				}
				utils::trim(username);
				if (api::GetUser(username, gamemode, &sp_data.user_info) == 0) {
					*params = u8"没这个人或者TA根本不玩这个模式！";
					return;
				}
				if (api::GetUserRecent(username, gamemode, &sp_data.score_info) == 0) {
					switch (gamemode) {
					case mode::std:
						*params = u8"TA最近在模式Standard上没有游玩记录~";
						return;
					case mode::taiko:
						*params = u8"TA最近在模式Taiko上没有游玩记录~";
						return;
					case mode::ctb:
						*params = u8"TA最近在模式Catch the Beat上没有游玩记录~";
						return;
					case mode::mania:
						*params = u8"TA最近在模式Mania上没有游玩记录~";
						return;
					}
				}
			}
			sp_data.mode = gamemode;
			api::GetBeatmap(sp_data.score_info.beatmap_id, &sp_data.beatmap_info);
			sprintf_s(beatmap_url, OSU_FILE_URL "%lld", sp_data.score_info.beatmap_id);
			beatmap = NetConnection::HttpsGet(beatmap_url);
			if (beatmap.empty()) {
				*params = 获取谱面信息错误;
				return;
			}
			int second, minute, maxsecond, maxminute;
			if (sp_data.mode == mode::std) {
				oppai pp;
				pp.read_data(beatmap);
				pp.mods((int)sp_data.score_info.mods);
				// if fc
				pp.accuracy_percent(-1);
				pp.n300((int)sp_data.score_info.n300);
				pp.n100((int)sp_data.score_info.n100);
				pp.n50((int)sp_data.score_info.n50);
				oppai_result t = pp.calc();
				sp_data.fc = (int)t.data.total_pp.value();
				// original
				pp.end((int)sp_data.score_info.n300 + (int)sp_data.score_info.n100 + (int)sp_data.score_info.n50
					+ (int)sp_data.score_info.nmiss);
				pp.nmiss((int)sp_data.score_info.nmiss);
				pp.combo((int)sp_data.score_info.combo);
				sp_data.pp_info = pp.calc();
				sp_data.pp_info.data.total_star = t.data.total_star;
				// reset
				pp.end(-1);
				pp.n300(-1);
				pp.n100(-1);
				pp.n50(-1);
				pp.nmiss(-1);
				pp.combo(-1);
				for (int i = 0; i < 5; ++i) {
					pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
					sp_data.confirmed_acc[i] = (int)(pp.calc().data.total_pp.value());
				}
				if (sp_data.pp_info.code == 0) {
					sp_data.pp_info.data.maxcombo = t.data.maxcombo;
					sp_data.pp_info.data.maxlength = t.data.length;
				}
				second = sp_data.pp_info.data.length / 1000 % 60;
				minute = sp_data.pp_info.data.length / 1000 / 60;
				maxsecond = sp_data.pp_info.data.maxlength / 1000 % 60;
				maxminute = sp_data.pp_info.data.maxlength / 1000 / 60;
			}
			else {
				if (sp_data.mode == mode::mania) {
					long n1, n2;
					n1 = sp_data.score_info.n50 * 50.0 + sp_data.score_info.n100 * 100.0
						+ sp_data.score_info.nkatu * 200.0
						+ (sp_data.score_info.n300 + sp_data.score_info.ngeki) * 300.0;
					n2 = 300
						* (sp_data.score_info.nmiss + sp_data.score_info.n50 + sp_data.score_info.nkatu
							+ sp_data.score_info.n100 + sp_data.score_info.n300 + sp_data.score_info.ngeki);
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == mode::taiko) {
					double n1, n2;
					n1 = ((double)sp_data.score_info.n100 + (double)sp_data.score_info.nkatu) * 0.5
						+ (double)sp_data.score_info.n300 + (double)sp_data.score_info.ngeki;
					n2 = (double)sp_data.score_info.nmiss + (double)sp_data.score_info.n100
						+ (double)sp_data.score_info.nkatu + (double)sp_data.score_info.n300
						+ (double)sp_data.score_info.ngeki;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == mode::ctb) {
					long n1, n2;
					n1 = sp_data.score_info.n50 + sp_data.score_info.n100 + sp_data.score_info.n300;
					n2 = sp_data.score_info.nkatu + sp_data.score_info.nmiss + sp_data.score_info.n50
						+ sp_data.score_info.n100 + sp_data.score_info.n300;
					sp_data.pp_info.data.maxcombo = sp_data.beatmap_info.maxcombo;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				second = 0;
				minute = 0;
				maxsecond = 0;
				maxminute = 0;
				sp_data.pp_info.data.titleUnicode = sp_data.beatmap_info.title;
				sp_data.pp_info.data.artistUnicode = sp_data.beatmap_info.artist;
				sp_data.pp_info.data.title = sp_data.beatmap_info.title;
				sp_data.pp_info.data.artist = sp_data.beatmap_info.artist;
				sp_data.pp_info.data.creator = sp_data.beatmap_info.creator;
				sp_data.pp_info.data.total_star = sp_data.beatmap_info.stars;
				sp_data.pp_info.data.bpm = sp_data.beatmap_info.bpm;
				sp_data.pp_info.data.ar = sp_data.beatmap_info.ar;
				sp_data.pp_info.data.od = sp_data.beatmap_info.od;
				sp_data.pp_info.data.cs = sp_data.beatmap_info.cs;
				sp_data.pp_info.data.hp = sp_data.beatmap_info.hp;
				sp_data.pp_info.data.combo = sp_data.score_info.combo;
			}
			char message[5120];
			string modeStr;
			sp_data.mode == mode::std
				? modeStr = "Standard"
				: sp_data.mode == mode::taiko
				? modeStr = "Taiko"
				: sp_data.mode == mode::ctb
				? modeStr = "CtB"
				: sp_data.mode == mode::mania ? modeStr = "Mania" : modeStr = "Unknown";

			if (sp_data.mode == mode::std) {
				sprintf_s(message,
					5120,
					u8"%s's previous play - %s\n"
					u8"%s - %s[%s]\n"
					"mapped by %s\n"
					"[ar%.1f | od%.1f | cs%.1f | hp%.1f | bpm%.2f]\n"
					"Stars:%.2f* | objects:%d | %d:%02d / %d:%02d\n"
					"300×%d | 100×%d | 50×%d | miss×%d\n"
					"combo %dx/%dx | acc %.2f%% | %s | %s\n"
					"%.0fpp | if fc %dpp\n\n"
					"100%% : %dpp\n"
					"99%% : %dpp\n"
					"98%% : %dpp\n"
					"95%% : %dpp\n%s",
					sp_data.user_info.username.c_str(),
					modeStr.c_str(),
					sp_data.pp_info.data.artistUnicode.c_str(),
					sp_data.pp_info.data.titleUnicode.c_str(),
					sp_data.pp_info.data.map_version.c_str(),
					sp_data.pp_info.data.creator.c_str(),
					sp_data.pp_info.data.ar,
					sp_data.pp_info.data.od,
					sp_data.pp_info.data.cs,
					sp_data.pp_info.data.hp,
					sp_data.pp_info.data.bpm,
					sp_data.pp_info.data.total_star,
					sp_data.pp_info.data.n300,
					minute,
					second,
					maxminute,
					maxsecond,
					sp_data.score_info.n300,
					sp_data.score_info.n100,
					sp_data.score_info.n50,
					sp_data.score_info.nmiss,
					sp_data.score_info.combo,
					sp_data.beatmap_info.maxcombo,
					sp_data.pp_info.data.accuracy,
					sp_data.score_info.mods == 0 ? "No Mod"
					: osu_api_v1::api::modParser(sp_data.score_info.mods).c_str(),
					sp_data.score_info.rank.c_str(),
					sp_data.pp_info.data.total_pp.has_value() ? sp_data.pp_info.data.total_pp.value() : 0.0,
					sp_data.fc,
					sp_data.confirmed_acc[4],
					sp_data.confirmed_acc[3],
					sp_data.confirmed_acc[2],
					sp_data.confirmed_acc[0],
					("https://osu.ppy.sh/b/" + to_string(sp_data.beatmap_info.beatmap_id)).c_str());
			}
			else {
				if (sp_data.mode == mode::ctb) {
					sprintf_s(
						message,
						5120,
						u8"%s's previous play - %s\n"
						u8"%s - %s[%s]\n"
						"mapped by %s\n"
						"[ar%.1f | od%.1f | cs%.1f | hp%.1f | bpm%.2f]\n"
						"Stars:%.2f* | objects:%d | %d:%02d / %d:%02d\n"
						"combo %dx/%dx | acc %.2f%% | %s | %s\n\n%s",
						sp_data.user_info.username.c_str(),
						modeStr.c_str(),
						sp_data.pp_info.data.artistUnicode.length() > 3 ? sp_data.pp_info.data.artistUnicode.c_str()
						: sp_data.pp_info.data.artist.c_str(),
						sp_data.pp_info.data.titleUnicode.length() > 3 ? sp_data.pp_info.data.titleUnicode.c_str()
						: sp_data.pp_info.data.title.c_str(),
						sp_data.pp_info.data.map_version.c_str(),
						sp_data.pp_info.data.creator.c_str(),
						sp_data.pp_info.data.ar,
						sp_data.pp_info.data.od,
						sp_data.pp_info.data.cs,
						sp_data.pp_info.data.hp,
						sp_data.pp_info.data.bpm,
						sp_data.pp_info.data.total_star,
						sp_data.pp_info.data.n300,
						minute,
						second,
						maxminute,
						maxsecond,
						sp_data.score_info.combo,
						sp_data.beatmap_info.maxcombo,
						sp_data.pp_info.data.accuracy,
						sp_data.score_info.mods == 0 ? "No Mod"
						: osu_api_v1::api::modParser(sp_data.score_info.mods).c_str(),
						sp_data.score_info.rank.c_str(),
						("https://osu.ppy.sh/b/" + to_string(sp_data.beatmap_info.beatmap_id)).c_str());
				}
				else {
					sprintf_s(
						message,
						5120,
						u8"%s's previous play - %s\n"
						u8"%s - %s[%s]\n"
						"mapped by %s\n"
						"[ar%.1f | od%.1f | cs%.1f | hp%.1f | bpm%.2f]\n"
						"Stars:%.2f* | objects:%d | %d:%02d / %d:%02d\n"
						"combo %dx | acc %.2f%% | %s | %s\n\n%s",
						sp_data.user_info.username.c_str(),
						modeStr.c_str(),
						sp_data.pp_info.data.artistUnicode.length() > 3 ? sp_data.pp_info.data.artistUnicode.c_str()
						: sp_data.pp_info.data.artist.c_str(),
						sp_data.pp_info.data.titleUnicode.length() > 3 ? sp_data.pp_info.data.titleUnicode.c_str()
						: sp_data.pp_info.data.title.c_str(),
						sp_data.pp_info.data.map_version.c_str(),
						sp_data.pp_info.data.creator.c_str(),
						sp_data.pp_info.data.ar,
						sp_data.pp_info.data.od,
						sp_data.pp_info.data.cs,
						sp_data.pp_info.data.hp,
						sp_data.pp_info.data.bpm,
						sp_data.pp_info.data.total_star,
						sp_data.pp_info.data.n300,
						minute,
						second,
						maxminute,
						maxsecond,
						sp_data.score_info.combo,
						sp_data.pp_info.data.accuracy,
						sp_data.score_info.mods == 0 ? "No Mod"
						: osu_api_v1::api::modParser(sp_data.score_info.mods).c_str(),
						sp_data.score_info.rank.c_str(),
						("https://osu.ppy.sh/b/" + to_string(sp_data.beatmap_info.beatmap_id)).c_str());
				}
			}
			*params = message;
			db.UpdatePPRecord(tar.user_id, sp_data.beatmap_info.beatmap_id);
		}
		static void setmode(string cmd, Target tar, string* params) {
			utils::string_replace(cmd, " ", "");
			if (!utils::isNum(cmd)) {
				*params = 英文模式名提示;
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			if (db.SetUserMainMode(tar.user_id, stoi(cmd)) == 0) {
				*params = u8"在执行操作时发生了错误..请稍后再试。";
				return;
			}
			switch (stoi(cmd)) {
			case 0:
				*params = u8"你的主要游戏模式已设置为Standard.";
				break;
			case 1:
				*params = u8"你的主要游戏模式已设置为Taiko.";
				break;
			case 2:
				*params = u8"你的主要游戏模式已设置为Catch the Beat.";
				break;
			case 3:
				*params = u8"你的主要游戏模式已设置为Mania.";
				break;
			default:
				*params = u8"发生了未知错误";
				break;
			}
		}
		static void bphead_tail(string cmd, Target tar, string* params) {
			utils::trim(cmd);
			cmd = utils::unescape(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			int temp;
			mode gamemode;
			user_info UI = { 0 };
			vector<score_info> SI;
			int64_t userid;
			string username = "";
			Database db;
			db.Connect();
			db.addcallcount();
			if (cmd.length() > 0) {
				if (cmd[0] == ':') {
					userid = db.GetUserID(tar.user_id);
					if (userid == 0) {
						*params = 未绑定;
						return;
					}
					else {
						try {
							temp = stoi(cmd.substr(cmd.length() - 1));
							if (temp < 4 && temp > -1) {
								gamemode = (mode)temp;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(userid);
							}
						}
						catch (std::exception) {
							gamemode = (mode)db.GetUserDefaultGameMode(userid);
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							temp = stoi(cmd.substr(cmd.find(':') + 1));
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (mode)temp;
						}
						else {
							gamemode = mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							username = cmd;
						}
						if (username.empty()) {
							if (api::GetUser(userid, mode::std, &UI) == 0) {
								*params = 用户已被bancho封禁;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(UI.user_id);
							}
						}
						else {
							if (api::GetUser(cmd, mode::std, &UI) == 0) {
								*params = 未从bancho检索到用户;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(UI.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.GetUserID(tar.user_id);
				if (userid == 0) {
					*params = 未绑定;
					return;
				}
				else {
					gamemode = (mode)db.GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (api::GetUser(userid, gamemode, &UI) == 0) {
					*params = 用户已被bancho封禁;
					return;
				}
				if (api::GetUserBest(userid, 100, gamemode, SI) == 0) {
					*params = u8"多打一打这个模式再来使用此命令喔~\n✧*｡٩(ˊᗜˋ*)و✧*｡";
					return;
				}
			}
			else {
				if (username.length() > 20) {
					*params = 参数过长提示;
					return;
				}
				if (api::GetUserBest(username, 100, gamemode, SI) == 0) {
					*params = u8"多打一打这个模式再来使用此命令喔~\n✧*｡٩(ˊᗜˋ*)و✧*｡";
					return;
				}
			}
			if (SI.size() < 100) {
				*params = u8"多打一打这个模式再来使用此命令喔~\n✧*｡٩(ˊᗜˋ*)و✧*｡";
				return;
			}
			UI.mode = gamemode;
			string modestr;
			switch (UI.mode) {
			case 0:
				modestr = "Standard";
				break;
			case 1:
				modestr = "Taiko";
				break;
			case 2:
				modestr = "Ctb";
				break;
			case 3:
				modestr = "Mania";
				break;
			default:
				modestr = "Unknown";
				break;
			}
			char message[1024];
			double totalvalue = 0.0;
			for (int i = 0; i < 100; ++i) {
				totalvalue = totalvalue + SI[i].pp;
			}
			sprintf_s(message,
				1024,
				u8"%s - 模式 %s\n"
				u8"你的bp1有 %.2f pp，\n"
				u8"你的bp2有 %.2f pp，\n...\n"
				u8"你的bp99有 %.2f pp，\n"
				u8"你的bp100有 %.2f pp，\n"
				u8"你bp1与bp100相差了有 %.2f pp，\n"
				u8"你的bp榜上所有成绩的平均值是 %.2f pp。",
				UI.username.c_str(),
				modestr.c_str(),
				SI[0].pp,
				SI[1].pp,
				SI[98].pp,
				SI[99].pp,
				SI[0].pp - SI[99].pp,
				totalvalue / 100);
			*params = message;
		}
		static void ppvs(string cmd, Target tar, string* params) {
			utils::trim(cmd);
			cmd = utils::unescape(cmd);
			if (cmd.length() < 1) {
				*params = u8"请提供要对比的玩家ID";
				return;
			}
			if (cmd.length() > 20) {
				*params = 参数过长提示;
				return;
			}
			mode gamemode;
			UserPanelData UI1 = { 0 };
			UserPanelData UI2 = { 0 };
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t UserID = db.GetUserID(tar.user_id);
			if (UserID == 0) {
				*params = 未绑定;
				return;
			}
			if (api::GetUser(UserID, mode::std, &UI1.user_info) == 0) {
				*params = 用户已被bancho封禁;
				return;
			}
			if (api::GetUser(cmd, mode::std, &UI2.user_info) == 0) {
				*params = 未从bancho检索到用户;
				return;
			}
			float u1pp = db.GetUserPreviousPP(UI1.user_info.user_id);
			float u2pp = db.GetUserPreviousPP(UI2.user_info.user_id);
			UI1.user_info.updatepplus = false;
			UI2.user_info.updatepplus = false;
			//剥离对pp+的依赖
			/*if (u1pp == UI1.user_info.pp) {
				UI1.user_info.updatepplus = false;
				db.GetUserPreviousPPlus(UI1.user_info.user_id, &UI1.pplus_info);
			}
			else {
				UI1.user_info.updatepplus = true;
			}
			if (u2pp == UI2.user_info.pp) {
				UI2.user_info.updatepplus = false;
				db.GetUserPreviousPPlus(UI2.user_info.user_id, &UI2.pplus_info);
			}
			else {
				UI2.user_info.updatepplus = true;
			}*/
			string fileStr = "osucat\\" + ppvsimg(UI1, UI2);
			*params = u8"[CQ:image,file=" + fileStr + u8"]";
			_DelayDelTmpFile(to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr);
		}
		static void badgelist(string cmd, Target tar, string* params) {
			int64_t uid;
			Database db;
			db.Connect();
			db.addcallcount();
			uid = db.GetUserID(tar.user_id);
			if (uid == 0) {
				*params = 未绑定;
				return;
			}
			string message;
			int i;
			badgeSystem::main bsm;
			badgeSystem::main::badge badgeStr;
			vector<int> t = db.GetBadgeList(uid);
			if (t.size() == 0) {
				*params = u8"你还没有获得过奖章。";
				return;
			}
			message = u8"你拥有 " + to_string(t.size()) + u8" 块勋章\n";
			for (i = 0; i < t.size(); ++i) {
				i == t.size() - 1 ? message += bsm.getBadgeStr(t[i]) : message += bsm.getBadgeStr(t[i]) + "\n";
			}
			*params = message;
		}
		static void occost(string cmd, Target tar, string* params) {
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			Database db;
			db.Connect();
			db.addcallcount();
			string username = "";
			int64_t UserID;
			double a, c, z, p;
			UserPanelData UI = { 0 };
			if (cmd.length() > 1) {
				if (cmd.find("at,qq=") != string::npos) {
					UserID = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
					if (UserID == 0) {
						*params = 他人未绑定;
						return;
					}
				}
				else {
					username = cmd;
				}
			}
			else {
				UserID = db.GetUserID(tar.user_id);
				if (UserID == 0) {
					*params = 未绑定;
					return;
				}
			}
			if (username.empty()) {
				if (api::GetUser(UserID, mode::std, &UI.user_info) == 0) {
					*params = 用户已被bancho封禁;
					return;
				}
			}
			else {
				if (username.length() > 20) {
					*params = 参数过长提示;
					return;
				}
				if (api::GetUser(username, mode::std, &UI.user_info) == 0) {
					*params = 未从bancho检索到用户;
					return;
				}
			}
			// 自PP+停止服务之后 均使用缓存读取内容
			//p = db.GetUserPreviousPP(UI.user_info.user_id);
			//if (UI.user_info.pp != p) {
			//    vector<long> pp_plus;
			//    try {
			//        pp_plus = NetConnection::getUserPlusData(UI.user_info.user_id);
			//        pplus_info pi;
			//        pi.acc = pp_plus[0];
			//        pi.flow = pp_plus[1];
			//        pi.jump = pp_plus[2];
			//        pi.pre = pp_plus[3];
			//        pi.spd = pp_plus[4];
			//        pi.sta = pp_plus[5];
			//        db.UpdatePPlus(UI.user_info.user_id, UI.user_info.pp, pi);
			//        p = UI.user_info.pp;
			//    } catch (std::exception) {
			//    }
			//}
			p = UI.user_info.pp;
			db.GetUserPreviousPPlus(UI.user_info.user_id, &UI.pplus_info);
			z = 1.92 * pow(UI.pplus_info.jump, 0.953) + 69.7 * pow(UI.pplus_info.flow, 0.596)
				+ 0.588 * pow(UI.pplus_info.spd, 1.175) + 3.06 * pow(UI.pplus_info.sta, 0.993);
			a = pow(UI.pplus_info.acc, 1.2768) * pow(p, 0.88213);
			c = min(0.00930973 * pow(p / 1000, 2.64192) * pow(z / 4000, 1.48422), 7) + min(a / 7554280, 3);
			char message[512];
			sprintf_s(message, 512, u8"在猫猫杯S1中，%s 的cost为：%.2f", UI.user_info.username.c_str(), c);
			*params = message;
		}
		static void searchuid(string cmd, Target tar, string* params) {
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			Database db;
			db.Connect();
			db.addcallcount();
			string username = "";
			int64_t UserID;
			double a, c, z, p;
			UserPanelData UI = { 0 };
			if (cmd.length() > 1) {
				if (cmd.find("at,qq=") != string::npos) {
					UserID = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
					if (UserID == 0) {
						*params = 他人未绑定;
						return;
					}
				}
				else {
					username = cmd;
				}
			}
			else {
				UserID = db.GetUserID(tar.user_id);
				if (UserID == 0) {
					*params = 未绑定;
					return;
				}
			}
			if (username.empty()) {
				if (api::GetUser(UserID, mode::std, &UI.user_info) == 0) {
					*params = 用户已被bancho封禁;
					return;
				}
			}
			else {
				if (username.length() > 20) {
					*params = 参数过长提示;
					return;
				}
				if (api::GetUser(username, mode::std, &UI.user_info) == 0) {
					*params = 未从bancho检索到用户;
					return;
				}
			}
			*params = UI.user_info.username + u8" 的osu!uid为 " + to_string(UI.user_info.user_id);
		}
		static void bonuspp(string cmd, Target tar, string* params) {
			Database db;
			db.Connect();
			db.addcallcount();
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			vector<score_info> bp;
			user_info UI = { 0 };
			int temp;
			mode gamemode;
			int64_t userid;
			string username = "";
			if (cmd.length() > 0) {
				if (cmd[0] == ':') {
					userid = db.GetUserID(tar.user_id);
					if (userid == 0) {
						*params = 未绑定;
						return;
					}
					else {
						try {
							temp = stoi(cmd.substr(cmd.length() - 1));
							if (temp < 4 && temp > -1) {
								gamemode = (mode)temp;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(userid);
							}
						}
						catch (std::exception) {
							gamemode = (mode)db.GetUserDefaultGameMode(userid);
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							temp = stoi(cmd.substr(cmd.find(':') + 1));
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (mode)temp;
						}
						else {
							gamemode = mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.GetUserID(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								*params = 他人未绑定;
								return;
							}
						}
						else {
							username = cmd;
						}
						if (username.empty()) {
							if (api::GetUser(userid, mode::std, &UI) == 0) {
								*params = 用户已被bancho封禁;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(UI.user_id);
							}
						}
						else {
							utils::trim(cmd);
							if (api::GetUser(cmd, mode::std, &UI) == 0) {
								*params = 未从bancho检索到用户;
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(UI.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.GetUserID(tar.user_id);
				if (userid == 0) {
					*params = 未绑定;
					return;
				}
				else {
					gamemode = (mode)db.GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (api::GetUser(userid, gamemode, &UI) == 0) {
					*params = 用户已被bancho封禁;
					return;
				}
				if (api::GetUserBest(userid, 100, gamemode, bp) == 0) {
					*params = u8"你在此模式上还没有成绩呢。";
					return;
				}
			}
			else {
				if (username.length() > 20) {
					*params = 参数过长提示;
					return;
				}
				utils::trim(username);
				if (api::GetUser(username, gamemode, &UI) == 0) {
					*params = 未从bancho检索到用户;
					return;
				}
				if (api::GetUserBest(username, 100, gamemode, bp) == 0) {
					*params = u8"他在此模式上还没有成绩呢。";
					return;
				}
			}
			double extraPolatePP;
			if (bp.size() < 100) {
				extraPolatePP = 0.0;
			}
			double scorepp = 0.0, _bonuspp = 0.0, pp = 0.0, sumOxy = 0.0, sumOx2 = 0.0, avgX = 0.0, avgY = 0.0,
				sumX = 0.0;
			for (int i = 0; i < bp.size(); ++i) {
				scorepp += bp[i].pp * pow(0.95, i);
			}
			vector<double> ys;
			for (int i = 0; i < bp.size(); i++) {
				ys.push_back(log10(bp[i].pp * pow(0.95, i)) / log10(100));
			}
#pragma region calculateLinearRegression
			for (int n = 1; n <= ys.size(); n++) {
				double weight = log1p(n + 1.0);
				sumX += weight;
				avgX += n * weight;
				avgY += ys[n - 1] * weight;
			}
			avgX /= sumX;
			avgY /= sumX;
			for (int n = 1; n <= ys.size(); n++) {
				sumOxy += (n - avgX) * (ys[n - 1] - avgY) * log1p(n + 1.0);
				sumOx2 += pow(n - avgX, 2.0) * log1p(n + 1.0);
			}
			double Oxy = sumOxy / sumX;
			double Ox2 = sumOx2 / sumX;
#pragma endregion
			double b[] = { avgY - (Oxy / Ox2) * avgX, Oxy / Ox2 };

			for (double n = 100; n <= UI.playcount; n++) {
				double val = pow(100.0, b[0] + b[1] * n);
				if (val <= 0.0) {
					break;
				}
				pp += val;
			}
			scorepp = scorepp + pp;
			_bonuspp = UI.pp - scorepp;
			int totalscores = UI.count_a + UI.count_s + UI.count_sh + UI.count_ss + UI.count_ssh;
			bool max;
			if (totalscores >= 25397 || _bonuspp >= 416.6667) {
				max = true;
			}
			else
				max = false;
			int rankedscores =
				max ? max(totalscores, 25397) : (int)round(log10(-(_bonuspp / 416.6667) + 1.0) / log10(0.9994));
			if (_isnan(scorepp) || _isnan(_bonuspp)) {
				scorepp = 0.0;
				_bonuspp = 0.0;
				rankedscores = 0;
			}
			string gamemodeStr;
			switch (gamemode) {
			case mode::std:
				gamemodeStr = "osu!Standard";
				break;
			case mode::taiko:
				gamemodeStr = "osu!Taiko";
				break;
			case mode::ctb:
				gamemodeStr = "osu!Catch the Beat";
				break;
			case mode::mania:
				gamemodeStr = "osu!Mania";
				break;
			default:
				gamemodeStr = "error";
				break;
			};
			char rtnmessage[1024];
			sprintf_s(rtnmessage,
				1024,
				u8"%s  (%s)\n总PP：%.2f\n原始PP：%.2f\nBonus PP：%.2f\n共计算出 %d 个被记录的ranked谱面成绩。",
				UI.username.c_str(),
				gamemodeStr.c_str(),
				UI.pp,
				scorepp,
				_bonuspp,
				rankedscores);
			*params = rtnmessage;
		}
		static void setbadge(string cmd, Target tar, string* params) {
			int64_t uid;
			Database db;
			db.Connect();
			db.addcallcount();
			uid = db.GetUserID(tar.user_id);
			if (uid == 0) {
				*params = 未绑定;
				return;
			}
			utils::trim(cmd);
			if (!utils::isNum(cmd)) {
				*params = u8"请提供ID";
				return;
			}
			int badgeID;
			try {
				badgeID = stoi(cmd);
			}
			catch (std::exception) {
				badgeID = 1025;
			}
			if (badgeID > 1024) {
				*params = u8"提供的ID有误";
				return;
			}
			badgeSystem::main bsm;
			badgeSystem::main::badge badgeStr;
			vector<int> t = db.GetBadgeList(uid);
			if (t.size() < 1) {
				*params = u8"你还没有获得任何奖章呢。";
				return;
			}
			for (int i = 0; i < t.size(); ++i) {
				if (badgeID == t[i]) {
					db.setshownBadges(uid, badgeID);
					*params = u8"你的主显奖章已设为 " + to_string(badgeID);
					return;
				}
			}
			*params = u8"你未拥有此奖章。";
		}
		static void setbanner_v1(string cmd, Target tar, string* params) {
			if (cmd.find("[CQ:image,file=") == string::npos) {
				*params = 自定义Banner帮助;
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t UserID = db.GetUserID(tar.user_id);
			if (UserID == 0) {
				*params = 未绑定;
				return;
			}
			int64_t QQ = db.GetQQ(UserID);
			string picPath;
			picPath = utils::GetMiddleText(cmd, "[CQ:image,file=", ",url");
			picPath = picPath.substr(picPath.find(',') + 6);
			PictureInfo picInfo = getImage(picPath);
			picPath = ".\\data\\cache\\" + picPath + "." + picInfo.format;
			if (!utils::fileExist(picPath)) {
				*params = 接收图片出错;
				return;
			}
			DrawableList dl;
			Image cover(picPath);
			Image coverRoundrect(Geometry(1200, 350), Color("none"));
			cover.resize(Geometry(1200, 1000000));
			cover.crop(Geometry(1200, 350));
			dl.clear();
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableRoundRectangle(0, 0, 1200, 350, 20, 20));
			coverRoundrect.draw(dl);
			coverRoundrect.composite(cover, 0, 0, InCompositeOp);
			coverRoundrect.quality(100);
			string filepath =
				".\\data\\images\\osucat\\custom\\banner_verify\\" + to_string(UserID) + ".jpg";
			coverRoundrect.write(filepath);
			//baiduaip::imageBase64(filepath);
			*params = 已上传待审核提示;
			Target activepushTar;
			activepushTar.message_type = Target::MessageType::PRIVATE;
			activepushTar.user_id = MONO;
			activepushTar.message = u8"有一个新的banner被上传，操作者UID：" + to_string(UserID) + u8"\n操作者QQ：" + to_string(QQ)
				+ u8" ,请尽快审核哦。\r\nbanner内容：\r\n"
				+ "[CQ:image,file=" + filepath.substr(14) + "]";
			activepush(activepushTar);
		}
		static void setinfopanel_v1(string cmd, Target tar, string* params) {
			if (cmd.find("[CQ:image,file=") == string::npos) {
				*params = 自定义InfoPanel帮助;
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t UserID = db.GetUserID(tar.user_id);
			if (UserID == 0) {
				*params = 未绑定;
				return;
			}
			int64_t QQ = db.GetQQ(UserID);
			string picPath;
			picPath = utils::GetMiddleText(cmd, "[CQ:image,file=", ",url");
			picPath = picPath.substr(picPath.find(',') + 6);
			PictureInfo picInfo = getImage(picPath);
			picPath = ".\\data\\cache\\" + picPath + "." + picInfo.format;
			if (!utils::fileExist(picPath)) {
				*params = 接收图片出错;
				return;
			}
			DrawableList dl;
			Image infoPanel(picPath);
			if (infoPanel.size().height() != 857 || infoPanel.size().width() != 1200 || infoPanel.magick() != "PNG") {
				*params = InfoPanel数据错误提示;
				return;
			}
			if (!utils::copyFile(picPath,
				".\\data\\images\\osucat\\custom\\infopanel_verify\\"
				+ to_string(UserID) + ".png")) {
				*params = u8"发生了一个未知错误";
				return;
			}
			*params = 已上传待审核提示;
			Target activepushTar;
			activepushTar.message_type = Target::MessageType::PRIVATE;
			activepushTar.user_id = MONO;
			activepushTar.message = u8"有一个新的InfoPanel被上传，操作者UID：" + to_string(UserID) + u8"\n操作者QQ：" + to_string(QQ)
				+ u8" ,请尽快审核哦。\r\nInfoPanel内容：\r\n"
				+ "[CQ:image,file=osucat\\custom\\infopanel_verify\\" + to_string(UserID) + ".png]";
			activepush(activepushTar);
		}
		static void resetbanner_v1(string cmd, Target tar, string* params) {
			Database db;
			db.Connect();
			db.addcallcount();
			string UID = to_string(db.GetUserID(tar.user_id));
			string picPath = "./work/v1_cover/" + UID + ".jpg";
			DeleteFileA(picPath.c_str());
			*params = u8"已成功重置你的banner。";
		}
		static void resetinfopanel_v1(string cmd, Target tar, string* params) {
			Database db;
			db.Connect();
			db.addcallcount();
			string UID = to_string(db.GetUserID(tar.user_id));
			string picPath = "./work/v1_infopanel/" + UID + ".png";
			DeleteFileA(picPath.c_str());
			*params = u8"已成功重置你的info面板。";
		}
		static void switchfunction(string cmd, Target tar, SenderInfo sdr, string* params) {
			if (tar.message_type != Target::MessageType::GROUP) {
				*params = u8"此操作必须在群内完成。";
				return;
			}
			if (sdr.member_role != "member") {
				*params = u8"此操作需要管理员执行。";
				return;
			}
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			Database db;
			db.Connect();
			db.addcallcount();
			vector<string> temp = utils::string_split(cmd, ':');
			string 参数不正确 =
				u8"给定的参数不正确\n正确的参数例：{function}:{on/off}\n参数名：bp/rctpp/recent/entertainment",
				功能已开启 = u8"这项功能已经是启用状态了", 功能已关闭 = u8"这项功能已经处于禁用状态了",
				已提交 = u8"操作已成功提交。";
			if (temp.size() != 2) {
				*params = 参数不正确;
				return;
			}
			if (temp[0] == "bp") {
				if (temp[1] == "on") {
					if (db.isGroupEnable(tar.group_id, 1) == 1) {
						*params = 功能已开启;
						return;
					}
					db.changeGroupSettings(tar.group_id, 1, true);
					*params = 已提交;
					return;
				}
				if (temp[1] == "off") {
					if (db.isGroupEnable(tar.group_id, 1) == 0) {
						*params = 功能已关闭;
						return;
					}
					db.changeGroupSettings(tar.group_id, 1, false);
					*params = 已提交;
					return;
				}
				*params = 参数不正确;
				return;
			}
			if (temp[0] == "rctpp") {
				if (temp[1] == "on") {
					if (db.isGroupEnable(tar.group_id, 2) == 1) {
						*params = 功能已开启;
						return;
					}
					db.changeGroupSettings(tar.group_id, 2, true);
					*params = 已提交;
					return;
				}
				if (temp[1] == "off") {
					if (db.isGroupEnable(tar.group_id, 2) == 0) {
						*params = 功能已关闭;
						return;
					}
					db.changeGroupSettings(tar.group_id, 2, false);
					*params = 已提交;
					return;
				}
				*params = 参数不正确;
				return;
			}
			if (temp[0] == "recent") {
				if (temp[1] == "on") {
					if (db.isGroupEnable(tar.group_id, 3) == 1) {
						*params = 功能已开启;
						return;
					}
					db.changeGroupSettings(tar.group_id, 3, true);
					*params = 已提交;
					return;
				}
				if (temp[1] == "off") {
					if (db.isGroupEnable(tar.group_id, 3) == 0) {
						*params = 功能已关闭;
						return;
					}
					db.changeGroupSettings(tar.group_id, 3, false);
					*params = 已提交;
					return;
				}
				*params = 参数不正确;
				return;
			}
			if (temp[0] == "entertainment") {
				if (temp[1] == "on") {
					if (db.isGroupEnable(tar.group_id, 4) == 1) {
						*params = 功能已开启;
						return;
					}
					db.changeGroupSettings(tar.group_id, 4, true);
					*params = 已提交;
					return;
				}
				if (temp[1] == "off") {
					if (db.isGroupEnable(tar.group_id, 4) == 0) {
						*params = 功能已关闭;
						return;
					}
					db.changeGroupSettings(tar.group_id, 4, false);
					*params = 已提交;
					return;
				}
				*params = 参数不正确;
				return;
			}
			*params = 参数不正确;
			return;
		}
		/* 管理指令*/
		static void adoptbanner_v1(string cmd, Target tar, string* params) {
			string UserID = cmd;
			utils::trim(UserID);
			string picPath = ".\\data\\images\\osucat\\custom\\banner_verify\\" + UserID + ".jpg";
			if (utils::fileExist(picPath) == true) {
				if (utils::copyFile(
					".\\data\\images\\osucat\\custom\\banner_verify\\" + UserID
					+ ".jpg",
					"./work/v1_cover/" + UserID + ".jpg")
					== true) {
					DeleteFileA(picPath.c_str());
					Database db;
					db.Connect();
					int64_t QQ = db.GetQQ(stoll(UserID));
					Target userMsg;
					userMsg.message_type = Target::MessageType::PRIVATE;
					userMsg.user_id = QQ;
					userMsg.message = u8"你上传的Banner通过审核啦，可以使用info指令查看~";
					activepush(userMsg);
					*params = u8"ID：" + UserID + u8" ，已成功通知用户Banner已审核成功。";
				}
				else {
					*params = u8"在移动文件时发生了一个错误。";
				}
			}
			else {
				*params = u8"此用户的内容不在待审核清单内。";
			}
		}
		static void adoptinfopanel_v1(string cmd, Target tar, string* params) {
			string UserID = cmd;
			utils::trim(UserID);
			string picPath =
				".\\data\\images\\osucat\\custom\\infopanel_verify\\" + UserID + ".png";
			if (utils::fileExist(picPath) == true) {
				if (utils::copyFile(
					".\\data\\images\\osucat\\custom\\infopanel_verify\\"
					+ UserID + ".png",
					"./work/v1_infopanel/" + UserID + ".png")
					== true) {
					DeleteFileA(picPath.c_str());
					Database db;
					db.Connect();
					int64_t QQ = db.GetQQ(stoll(UserID));
					Target userMsg;
					userMsg.message_type = Target::MessageType::PRIVATE;
					userMsg.user_id = QQ;
					userMsg.message = u8"你上传的Info面板通过审核啦，可以使用info指令查看~";
					activepush(userMsg);
					*params = u8"ID：" + UserID + u8" ，已成功通知用户InfoPanel已审核成功。";
				}
				else {
					*params = u8"在移动文件时发生了一个错误。";
				}
			}
			else {
				*params = u8"此用户的内容不在待审核清单内。";
			}
		}
		static void rejectbanner_v1(string cmd, Target tar, string* params) {
			string UserID, Content;
			if (cmd.find('#') != string::npos) {
				string tmp = cmd;
				utils::trim(tmp);
				UserID = tmp.substr(0, tmp.find('#'));
				Content = tmp.substr(tmp.find("#") + 1);
				if (Content.length() < 1) {
					Content = u8"未提供详情。";
				}
			}
			else {
				UserID = cmd;
				utils::trim(UserID);
				Content = u8"未提供详情。";
			}
			string picPath = ".\\data\\images\\osucat\\custom\\banner_verify\\" + UserID + ".jpg";
			if (utils::fileExist(picPath) == true) {
				DeleteFileA(picPath.c_str());
				Database db;
				db.Connect();
				int64_t QQ = db.GetQQ(stoll(UserID));
				Target activepushTar;
				activepushTar.message_type = Target::MessageType::PRIVATE;
				activepushTar.user_id = QQ;
				activepushTar.message = u8"你上传的Banner已被管理员驳回，详情：" + Content;
				activepush(activepushTar);
				*params = "ID：" + UserID + u8" ，已成功通知用户Banner已被驳回。详情：" + Content;
			}
			else {
				*params = u8"此用户的内容不在待审核清单内。";
			}
		}
		static void rejectinfopanel_v1(string cmd, Target tar, string* params) {
			string UserID, Content;
			if (cmd.find('#') != string::npos) {
				string tmp = cmd;
				utils::trim(tmp);
				UserID = tmp.substr(0, tmp.find('#'));
				Content = tmp.substr(tmp.find("#") + 1);
				if (Content.length() < 1) {
					Content = u8"未提供详情。";
				}
			}
			else {
				UserID = cmd;
				utils::trim(UserID);
				Content = u8"未提供详情。";
			}
			string picPath = ".\\data\\images\\osucat\\custom\\infopanel_verify\\" + UserID + ".png";
			if (utils::fileExist(picPath) == true) {
				DeleteFileA(picPath.c_str());
				Database db;
				db.Connect();
				int64_t QQ = db.GetQQ(stoll(UserID));
				Target activepushTar;
				activepushTar.message_type = Target::MessageType::PRIVATE;
				activepushTar.user_id = QQ;
				activepushTar.message = u8"你上传的InfoPanel已被管理员驳回，详情：" + Content;
				activepush(activepushTar);
				*params = "ID：" + UserID + u8" ，已成功通知用户InfoPanel已被驳回。详情：" + Content;
			}
			else {
				*params = u8"此用户的内容不在待审核清单内。";
			}
		}
		static void resetuser(string cmd, Target tar, string* params) {
			string UserID, Content;
			if (cmd.find('#') != string::npos) {
				string tmp = cmd;
				utils::trim(tmp);
				UserID = tmp.substr(0, tmp.find('#'));
				Content = tmp.substr(tmp.find("#") + 1);
				if (Content.length() < 1) {
					Content = "未提供详情。";
				}
			}
			else {
				UserID = cmd;
				utils::trim(UserID);
				Content = "未提供详情。";
			}
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t QQ = db.GetQQ(stoll(UserID));
			if (!QQ == 0) {
				try {
					DeleteFileA(("./work/v1_cover/" + UserID + ".jpg").c_str());
				}
				catch (exception) {
				}
				try {
					DeleteFileA(("./work/v1_infopanel/" + UserID + ".jpg").c_str());
				}
				catch (exception) {
				}
				Target activepushTar;
				activepushTar.message_type = Target::MessageType::PRIVATE;
				activepushTar.user_id = QQ;
				activepushTar.message = u8"你的个人资料已被重置，详情：" + Content;
				activepush(activepushTar);
				*params = "ID：" + UserID + u8" ，已成功通知用户他的个人资料已被重置。详情：" + Content;
			}
			else {
				*params = u8"找不到此用户。";
			}
		}
		static void addbadge(string cmd, Target tar, string* params) {
			string args = cmd;
			utils::trim(args);
			int64_t uid;
			int badgeid;
			try {
				uid = stoll(args.substr(0, args.find(',')));
			}
			catch (std::exception) {
				*params = u8"参数错误";
				return;
			}
			try {
				badgeid = stoi(args.substr(args.find(',') + 1));
			}
			catch (std::exception) {
				*params = u8"参数错误";
				return;
			}
			if (badgeid >= (int)badgeSystem::main::badge::SELF_MAXIUMINDEX) {
				*params = u8"badge not found.";
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			string temp = "";
			vector<int> tmp = db.GetBadgeList(uid);
			if (tmp.size() > 0) {
				for (size_t i = 0; i < tmp.size(); ++i) {
					temp += to_string(tmp[i]) + ",";
				}
				temp += to_string(badgeid);
			}
			else {
				temp = to_string(badgeid);
			}
			db.addbadge(uid, temp);
			*params = u8"已成功提交。";
		}
		/* 娱乐功能 */
		static bool funStuff(string cmd, Target tar, SenderInfo senderinfo, string* params) {
			if (randRepeater()) { *params = cmd; return true; }//复读机
			return false;
		}
		static bool randRepeater() {
			//概率复读
			srand((unsigned)GetTickCount());
			if (1 == rand() % 100) {
				return true;
			}
			return false;
		}
		static void memyselfact(string cmd, Target tar, SenderInfo senderinfo, string* params) {
			utils::trim(cmd);
			string username = senderinfo.card == "" ? senderinfo.nikename : senderinfo.card;
			*params = username + " " + cmd;
			Database db;
			db.Connect();
			db.addcallcount();
		}
		/* 系统事件 */
		static void cleartemp(string cmd, Target tar, string* params) {
			utils::trim(cmd);
			if (cmd.length() < 3) {
				*params = "请提供参数。";
				return;
			}
			if (_stricmp(cmd.substr(0, 3).c_str(), "all") == 0) {
				system("del .\\work\\avatar\\*.png");
				*params = "已成功提交。";
				return;
			}
			if (_stricmp(cmd.substr(0, 6).c_str(), "avatar") == 0) {
				system("del .\\work\\avatar\\*.png");
				*params = "已成功提交。";
				return;
			}
			*params = "参数错误。";
		}
		static void _UpdateManually(Target tar) {
			char dugtmp[256];
			Target activepushTar;
			activepushTar.message_type = tar.message_type == Target::MessageType::PRIVATE ? Target::MessageType::PRIVATE : Target::MessageType::GROUP;
			tar.message_type == Target::MessageType::PRIVATE ? activepushTar.user_id = tar.user_id : activepushTar.group_id = tar.group_id;
			activepushTar.message = u8"正在启动更新";
			activepush(activepushTar);
			activepushTar.message_type = Target::MessageType::PRIVATE;
			activepushTar.user_id = MONO;
			activepushTar.message = u8"管理员 " + to_string(tar.user_id) + u8" 手动发起了每日更新 正在启动更新";
			activepush(activepushTar);
			sprintf_s(dugtmp, u8"[%s] [osucat][updater]：启动更新", utils::unixTime2Str(time(NULL)).c_str());
			cout << dugtmp << endl;
			auto start = chrono::system_clock::now();
			Database db;
			db.Connect();
			vector<int64_t> temp = db.GetUserSet();
			char timeStr[30] = { 0 };
			time_t now = time(NULL);
			tm* tm_now = localtime(&now);
			strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_now);
			for (int i = 0; i < temp.size(); ++i) {
				thread DUP(bind(&dailyUpdatePoster, i, temp[i], timeStr));
				DUP.detach();
				Sleep(2000);
			}
			auto end = chrono::system_clock::now();
			auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
			string message = u8"已完成，一共更新了 " + to_string(temp.size()) + u8" 个用户的数据，共耗时 "
				+ utils::Duration2StrWithoutDAY(double(duration.count()) * chrono::microseconds::period::num
					/ chrono::microseconds::period::den);
			activepushTar.message = message;
			activepush(activepushTar);
			activepushTar.message_type = tar.message_type == Target::MessageType::PRIVATE ? Target::MessageType::PRIVATE : Target::MessageType::GROUP;
			tar.message_type == Target::MessageType::PRIVATE ? activepushTar.user_id = tar.user_id : activepushTar.group_id = tar.group_id;
			activepush(activepushTar);
			sprintf_s(dugtmp, u8"[%s] [osucat][updater]：%s", utils::unixTime2Str(time(NULL)).c_str(), message.c_str());
		}
		static void _DailyUpdate() {
			cout << u8"Daily update thread created!" << endl;
			char dugtmp[256];
			while (true) {
				time_t now = time(NULL);
				tm* tm_now = localtime(&now);
				char timeC[16] = { 0 };
				strftime(timeC, sizeof(timeC), "%H", tm_now);
				if (to_string(timeC).find("04") == string::npos) {
					Sleep(1000 * 60);
				}
				else {
					Target activepushTar;
					activepushTar.message_type = Target::MessageType::PRIVATE;
					activepushTar.user_id = MONO;
					activepushTar.message = u8"正在启动更新";
					activepush(activepushTar);
					try {
						system("del .\\work\\avatar\\*.png");
					}
					catch (...) {
					}
					sprintf_s(dugtmp, u8"[%s] [osucat][updater]：启动更新", utils::unixTime2Str(time(NULL)).c_str());
					cout << dugtmp << endl;
					auto start = chrono::system_clock::now();
					Database db;
					db.Connect();
					vector<int64_t> temp = db.GetUserSet();
					char timeStr[30] = { 0 };
					strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_now);
					for (int i = 0; i < temp.size(); ++i) {
						thread DUP(bind(&dailyUpdatePoster, i, temp[i], timeStr));
						DUP.detach();
						Sleep(2000);
					}
					auto end = chrono::system_clock::now();
					auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
					string message = u8"已完成，一共更新了 " + to_string(temp.size()) + u8" 个用户的数据，共耗时 "
						+ utils::Duration2StrWithoutDAY(double(duration.count()) * chrono::microseconds::period::num
							/ chrono::microseconds::period::den);
					activepushTar.message = message;
					activepush(activepushTar);
					sprintf_s(dugtmp, u8"[%s] [osucat][updater]：%s", utils::unixTime2Str(time(NULL)).c_str(), message.c_str());
					Sleep(1000 * 60 * 60 * 21);
				}
			}
		}
		static void dailyUpdatePoster(int id, int64_t userid, const string& timeStr) {
			user_info UI = { 0 };
			Database db;
			db.Connect();

			try {
				if (api::GetUser(userid, osu_api_v1::mode::std, &UI) != 0) {
					try {
						db.AddUserData(&UI, timeStr);
					}
					catch (osucat::database_exception) {
					}
				}
			}
			catch (osucat::NetWork_Exception) {
				try {
					if (api::GetUser(userid, osu_api_v1::mode::std, &UI) != 0) {
						try {
							db.AddUserData(&UI, timeStr);
						}
						catch (osucat::database_exception) {
						}
					}
				}
				catch (osucat::NetWork_Exception) {
					try {
						if (api::GetUser(userid, osu_api_v1::mode::std, &UI) != 0) {
							try {
								db.AddUserData(&UI, timeStr);
							}
							catch (osucat::database_exception) {
							}
						}
					}
					catch (osucat::NetWork_Exception) {
					}
				}
			}

			try {
				if (api::GetUser(userid, osu_api_v1::mode::taiko, &UI) != 0) {
					try {
						db.AddUserData(&UI, timeStr);
					}
					catch (osucat::database_exception) {
					}
				}
			}
			catch (osucat::NetWork_Exception) {
				try {
					if (api::GetUser(userid, osu_api_v1::mode::taiko, &UI) != 0) {
						try {
							db.AddUserData(&UI, timeStr);
						}
						catch (osucat::database_exception) {
						}
					}
				}
				catch (osucat::NetWork_Exception) {
					try {
						if (api::GetUser(userid, osu_api_v1::mode::taiko, &UI) != 0) {
							try {
								db.AddUserData(&UI, timeStr);
							}
							catch (osucat::database_exception) {
							}
						}
					}
					catch (osucat::NetWork_Exception) {
					}
				}
			}

			try {
				if (api::GetUser(userid, osu_api_v1::mode::ctb, &UI) != 0) {
					try {
						db.AddUserData(&UI, timeStr);
					}
					catch (osucat::database_exception) {
					}
				}
			}
			catch (osucat::NetWork_Exception) {
				try {
					if (api::GetUser(userid, osu_api_v1::mode::ctb, &UI) != 0) {
						try {
							db.AddUserData(&UI, timeStr);
						}
						catch (osucat::database_exception) {
						}
					}
				}
				catch (osucat::NetWork_Exception) {
					try {
						if (api::GetUser(userid, osu_api_v1::mode::ctb, &UI) != 0) {
							try {
								db.AddUserData(&UI, timeStr);
							}
							catch (osucat::database_exception) {
							}
						}
					}
					catch (osucat::NetWork_Exception) {
					}
				}
			}

			try {
				if (api::GetUser(userid, osu_api_v1::mode::mania, &UI) != 0) {
					try {
						db.AddUserData(&UI, timeStr);
					}
					catch (osucat::database_exception) {
					}
				}
			}
			catch (osucat::NetWork_Exception) {
				try {
					if (api::GetUser(userid, osu_api_v1::mode::mania, &UI) != 0) {
						try {
							db.AddUserData(&UI, timeStr);
						}
						catch (osucat::database_exception) {
						}
					}
				}
				catch (osucat::NetWork_Exception) {
					try {
						if (api::GetUser(userid, osu_api_v1::mode::mania, &UI) != 0) {
							try {
								db.AddUserData(&UI, timeStr);
							}
							catch (osucat::database_exception) {
							}
						}
					}
					catch (osucat::NetWork_Exception) {
					}
				}
			}
			char dugtmp[256];
			sprintf_s(dugtmp, u8"[%s] [osucat][updater]：(No.%d) 已成功更新用户 %lld 的数据。",
				utils::unixTime2Str(time(NULL)).c_str(),
				id,
				userid);
			cout << dugtmp << endl;
		}
		static void _DelayDelTmpFile(string filename, int delayTime = 15) {
			//delayTime单位：秒
			thread DFH(bind(&DelFileHandler, filename, delayTime));
			DFH.detach();
		}
		static void DelFileHandler(string filename, int delayTime = 0) {
			/*
			可以设置delayTime来决定在几秒后删除
			*/
			if (delayTime > 0)Sleep(delayTime * 1000);
			DeleteFileA(filename.c_str());
		}
#pragma endregion
#pragma region 交互
		/*
		通过message_type来判断是群组消息还是好友消息
		message在这里等同于params
		*/
		static void activepush(Target tar) {
			if (tar.message_type == Target::MessageType::PRIVATE) {
				json jp;
				jp["user_id"] = tar.user_id;
				jp["message"] = tar.message;
				try {
					NetConnection::HttpPost("http://127.0.0.1:5700/send_private_msg", jp);
				}
				catch (osucat::NetWork_Exception& ex) {
					cout << ex.Show() << endl;
				}
				char msg[4096];
				sprintf_s(msg,
					u8"[%s] [osucat][↑]: 发送至好友 %lld 的消息：%s",
					utils::unixTime2Str(time(NULL)).c_str(),
					tar.user_id,
					tar.message.c_str());
				cout << msg << endl;
			}
			if (tar.message_type == Target::MessageType::GROUP) {
				json jp;
				jp["group_id"] = tar.group_id;
				jp["message"] = tar.message;
				try {
					NetConnection::HttpPost("http://127.0.0.1:5700/send_group_msg", jp);
				}
				catch (osucat::NetWork_Exception& ex) {
					cout << ex.Show() << endl;
				}
				char msg[4096];
				sprintf_s(msg,
					u8"[%s] [osucat][↑]: 发送至群 %lld 的消息：%s",
					utils::unixTime2Str(time(NULL)).c_str(),
					tar.group_id,
					tar.message.c_str());
				cout << msg << endl;
			}
		}
		static void eventActivePush(Request req) {
			if (req.request_type == Request::RequestType::FRIEND) {
				json jp;
				jp["flag"] = req.flag;
				jp["approve"] = true;
				try {
					NetConnection::HttpPost("http://127.0.0.1:5700/set_friend_add_request", jp);
				}
				catch (osucat::NetWork_Exception& ex) {
					cout << ex.Show() << endl;
				}
				char msg[4096];
				sprintf_s(msg,
					u8"[%s] [osucat][↑]: 已通过来自 %lld 的好友请求。",
					utils::unixTime2Str(time(NULL)).c_str(),
					req.user_id);
				cout << msg << endl;
			}
			if (req.request_type == Request::RequestType::GROUP) {
				json jp;
				jp["flag"] = req.flag;
				jp["sub_type"] = req.GR_SubType;
				jp["approve"] = true;
				try {
					NetConnection::HttpPost("http://127.0.0.1:5700/set_group_add_request", jp);
				}
				catch (osucat::NetWork_Exception& ex) {
					cout << ex.Show() << endl;
				}
				char msg[4096];
				sprintf_s(msg,
					u8"[%s] [osucat][↑]: 已通过来自好友 %lld 的群 %lld 邀请请求。",
					utils::unixTime2Str(time(NULL)).c_str(),
					req.user_id,
					req.group_id);
				cout << msg << endl;
			}
		}
		static PictureInfo getImage(string file_name) {
			json jp;
			jp["file"] = file_name;
			PictureInfo rtn;
			rtn.size = 0;
			rtn.filename = "";
			rtn.url = "";
			rtn.format = "";
			try {
				string data = NetConnection::HttpPost("http://127.0.0.1:5700/get_image", jp);
				cout << data << endl;
				json j = json::parse(data)["data"];
				rtn.size = j["size"].get<int32_t>();
				rtn.filename = j["filename"].get<string>();
				rtn.url = j["url"].get<string>();
				rtn.format = rtn.filename.substr(rtn.filename.length() - 4);
			}
			catch (osucat::NetWork_Exception& ex) {
				cout << ex.Show() << endl;
			}
			char msg[4096];
			sprintf_s(msg,
				u8"[%s] [osucat][↓]: 接收图片 %s | 图片格式：%s | 大小: %ld | URL: %s",
				utils::unixTime2Str(time(NULL)).c_str(),
				rtn.filename.c_str(),
				rtn.format,
				rtn.size,
				rtn.url.c_str());
			cout << msg << endl;
			return rtn;
		}
#pragma endregion
	};
}