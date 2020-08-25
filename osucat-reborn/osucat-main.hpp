#pragma once

using json = nlohmann::json;
using namespace std;

namespace osucat {
	class main {
	public:
		static void monitors() {
			using easywsclient::WebSocket;
			cout << u8"ws客户端准备就绪...等待连接中..." << endl;
			std::unique_ptr<WebSocket> ws(WebSocket::from_url("ws://localhost:6700/"));
			if (ws == false) {
				printf(u8"尝试连接到websocket服务器失败。");
				return;
			}
			cout << u8"连接成功，正在监听..." << endl;
			if (DEBUGMODE) {
				cout << u8"Debug模式已启用..." << endl;
			}
			while (true) {
				WebSocket::pointer wsp = &*ws;
				ws->poll();
				ws->dispatch([wsp](const std::string& message) {
					if (DEBUGMODE) {
						cout << message << "\n" << endl;
					}
					if (_stricmp(message.substr(0, 6).c_str(), "{\"data") != 0) { //忽略回执消息
						json j = json::parse(message);
						/*
						Message事件处理
						*/
						if (j["post_type"].get<string>() == "message") {
							json sj = json::parse(message)["sender"];
							Target tar;
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
									string params = { 0 };
									if (cmdParse(str, tar, &params)) {
										json jp;
										jp["action"] = "send_msg";
										jp["params"] = {
											{"message_type","private"},
											{"user_id",tar.user_id},
											{"message",params}
										};
										jp["echo"] = "success";
										wsp->send(jp.dump());
									}
								}
							}
							if (tar.message_type == Target::MessageType::GROUP) {
								sprintf_s(msg, u8"[%s] [osucat]: 收到来自群 %lld 的 %lld 的消息：%s", utils::unixTime2Str(tar.time).c_str(), tar.group_id, tar.user_id, tar.message.c_str());
								cout << msg << endl;
								if (tar.message[0] == '!' || tar.message.find(u8"！") == 0) {
									string str = tar.message;
									str = tar.message[0] < 0 ? tar.message.substr(3) : tar.message.substr(1);
									string params = { 0 };
									if (cmdParse(str, tar, &params)) {
										Database db;
										db.Connect();
										db.addcallcount();
										json jp;
										jp["action"] = "send_msg";
										jp["params"] = {
											{"message_type","group"},
											{"group_id",tar.group_id},
											{"message",params}
										};
										jp["echo"] = "success";
										wsp->send(jp.dump());
									}
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
								//req.request_type = Request::RequestType::FRIEND;
								char msg[4096];
								sprintf_s(msg, u8"[%s] [osucat]: 收到来自用户 %lld 的好友请求：%s\n", utils::unixTime2Str(req.time).c_str(), req.user_id, req.message.c_str());
								cout << msg << endl;
								json jp;
								jp["action"] = "set_friend_add_request";
								jp["params"] = {
									{"flag",req.flag},
									{"approve",true}
								};
								jp["echo"] = "success";
								wsp->send(jp.dump());
							}
							if (j["request_type"].get<string>() == "group") {
								//req.request_type = Request::RequestType::GROUP;
								if (j["sub_type"].get<string>() == "invite") {
									//只有受邀入群才会被处理
									char msg[4096];
									sprintf_s(msg, u8"[%s] [osucat]: 收到来自用户 %lld 的群组邀请请求：%s\n", utils::unixTime2Str(req.time).c_str(), req.user_id, req.message.c_str());
									cout << msg << endl;
									json jp;
									jp["action"] = "set_group_add_request";
									jp["params"] = {
										{"flag",req.flag},
										{"sub_type","invite"},
										{"approve",true}
									};
									jp["echo"] = "success";
									wsp->send(jp.dump());
								}
							}
						}
					}
					//wsp->close();
					});
			}
		}
		static bool cmdParse(string msg, Target tar, string* params) {
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
			return false;
		}
		//done
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
				*params = u8"你在干嘛...";
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
					*params = u8"你的账户已经被bancho ban掉了，请先解绑再绑定新的账户。";
				}
				return;
			}
			if (api::GetUser(cmd, osu_api_v1::mode::std, &UI) == 0) {
				*params = u8"没这个人！";
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
				*params = u8"没这个人！";
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
			info("", tar , params);
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
					*params = u8"已成功解绑。";
					Target activepushTar;
					activepushTar.message_type = Target::MessageType::PRIVATE;
					activepushTar.user_id = 451577581;
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
			imageMaker::UserPanelData upd = { 0 };
			user_info previousUserInfo = { 0 };
			int64_t userid;
			string username = "";
			gamemode = mode::std;
			if (cmd.length() > 0) {
				if (cmd[0] == '#') {
					userid = db.GetUserID(tar.user_id);
					if (userid == 0) {
						*params = u8"你还没有绑定你的账户，请使用!setid id来绑定~";
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
							*params = u8"你还没有绑定你的账户，请使用!setid id来绑定~";
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
							*params = u8"你还没有绑定你的账户，请使用!setid id来绑定~";
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
								*params = u8"他还没有绑定他的账户，请让他使用!setid id来绑定哦——";
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
								*params = u8"他还没有绑定他的账户，请让他使用!setid id来绑定哦——";
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
								*params = u8"被办了。";
								return;
							}
							else {
								gamemode = (mode)db.GetUserDefaultGameMode(upd.user_info.user_id);
							}
						}
						else {
							if (username.length() > 20) {
								*params = u8"你在干嘛...";
								return;
							}
							utils::trim(username);
							if (api::GetUser(username, mode::std, &upd.user_info) == 0) {
								*params = u8"没这个人！";
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
					*params = u8"你还没有绑定你的账户，请使用!setid id来绑定~";
					return;
				}
				else {
					gamemode = (mode)db.GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (api::GetUser(userid, gamemode, &upd.user_info) == 0) {
					*params = u8"被办了。";
					return;
				}
			}
			else {
				if (username.length() > 20) {
					*params = u8"你在干嘛...";
					return;
				}
				utils::trim(username);
				if (api::GetUser(username, gamemode, &upd.user_info) == 0) {
					*params = u8"没这个人！";
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
					fileStr = "osucat/" + imageMaker::infoPic_v1(upd, previousUserInfo, true);
				}
				else {
					db.GetUserData(upd.user_info.user_id, 0, &previousUserInfo);
					fileStr = "osucat/" + infoPic_v1(upd, previousUserInfo, true);
					// infoPic_v2(upd, previousUserInfo, true);
				}
			}
			else {
				fileStr = "osucat/" + infoPic_v1(upd, upd.user_info);
			}
			*params = u8"[CQ:image,file=osucat\\"+ fileStr + u8"]";
			DeleteFileA(("osucat\\" + fileStr).c_str());
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
	};
}