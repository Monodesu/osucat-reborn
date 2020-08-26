#pragma once

#include "message.h"

using json = nlohmann::json;
using namespace std;

namespace osucat {
	class main {
	public:
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
						sprintf_s(msg, u8"[%s] [osucat]: 收到来自好友 %lld 的消息：%s", utils::unixTime2Str(tar.time).c_str(), tar.user_id, tar.message.c_str());
						cout << msg << endl;
						if (tar.message[0] == '!' || tar.message.find(u8"！") == 0) {
							string str = tar.message;
							str = tar.message[0] < 0 ? tar.message.substr(3) : tar.message.substr(1);
							monitors(str, tar, sdr);

						}
					}
					if (tar.message_type == Target::MessageType::GROUP) {
						sprintf_s(msg, u8"[%s] [osucat]: 收到来自群 %lld 的 %lld 的消息：%s", utils::unixTime2Str(tar.time).c_str(), tar.group_id, tar.user_id, tar.message.c_str());
						cout << msg << endl;
						if (tar.message[0] == '!' || tar.message.find(u8"！") == 0) {
							string str = tar.message;
							str = tar.message[0] < 0 ? tar.message.substr(3) : tar.message.substr(1);
							monitors(str, tar, sdr);
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
						sprintf_s(msg, u8"[%s] [osucat]: 收到来自用户 %lld 的好友请求：%s\n", utils::unixTime2Str(req.time).c_str(), req.user_id, req.message.c_str());
						cout << msg << endl;
						eventActivePush(req);
					}
					if (j["request_type"].get<string>() == "group") {
						req.request_type = Request::RequestType::GROUP;
						req.GR_SubType = j["sub_type"].get<string>();
						if (j["sub_type"].get<string>() == "invite") {
							//只有受邀入群才会被处理
							char msg[4096];
							sprintf_s(msg, u8"[%s] [osucat]: 收到来自用户 %lld 的群组邀请请求：%s\n", utils::unixTime2Str(req.time).c_str(), req.user_id, req.message.c_str());
							cout << msg << endl;
							eventActivePush(req);
						}
					}
				}
			}
		}
		static void monitors(string msg, Target tar, SenderInfo senderinfo) {
			string params = { 0 };
			if (cmdParse(msg, tar, senderinfo, &params)) {
				Target activepushTar;
				activepushTar.message_type = tar.message_type == Target::MessageType::PRIVATE ? Target::MessageType::PRIVATE : Target::MessageType::GROUP;
				tar.message_type == Target::MessageType::PRIVATE ? activepushTar.user_id = tar.user_id : activepushTar.group_id = tar.group_id;
				activepushTar.message = params;
				activepush(activepushTar);
			}
		}
#pragma endregion
#pragma region 指令解析
		static bool cmdParse(string msg, Target tar, SenderInfo senderinfo, string* params) {
			try {
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

				if (_stricmp(msg.substr(0, 2).c_str(), "pp") == 0) {
					pp(msg.substr(2), tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 4).c_str(), "with") == 0) {
					ppwith(msg.substr(4), tar, params);
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
				// 拦截其他娱乐模块 //
#pragma region 娱乐模块
				Database db;
				db.Connect();
				if (tar.message_type == Target::MessageType::GROUP) {
					if (db.isGroupEnable(tar.group_id, 4) == 0) {
						return false;
					}
				}
				/*if (_stricmp(msg.substr(0, 2).c_str(), "me") == 0) {
					memyselfact(msg.substr(2), tar, senderinfo, params);
					return true;
				}*/
#pragma endregion
			}
			catch (osucat::database_exception& ex) {
				cout << ex.Show() << endl;
				cout << ex.Code() << endl;
			}
			catch (osucat::NetWork_Exception& ex) {
				cout << ex.Show() << endl;
				cout << ex.Code() << endl;
			}
			catch (std::exception& ex) {
				cout << ex.what() << endl;
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
			activepushTar.user_id = 451577581;
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
				UI.username.c_str());
			*params = return_message;
			Target activepushTar;
			activepushTar.message_type = Target::MessageType::PRIVATE;
			activepushTar.user_id = 451577581;
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
			//不能立即删除 需要一个延时方案
			//DeleteFileA((to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr).c_str());
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
			//DeleteFileA((cq::dir::root("data", "image") + fileStr).c_str());
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
			//DeleteFileA((dir::root("data", "image") + fileStr).c_str());
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
			//DeleteFileA((cq::dir::root("data", "image") + fileStr).c_str());
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
		//Todo...
		/* 娱乐模块 */
		static void memyselfact(string cmd, Target tar, SenderInfo senderinfo, string* params) {
			utils::trim(cmd);
			string username = senderinfo.card == "" ? senderinfo.nikename : senderinfo.card;
			*params = username + " " + cmd;
			Database db;
			db.Connect();
			db.addcallcount();
		}
	private:
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
			}
		}
	};
}