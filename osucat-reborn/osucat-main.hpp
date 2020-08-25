#pragma once

using json = nlohmann::json;
using namespace std;

namespace osucat {
	class main {
	public:
		static void monitors() {
			using easywsclient::WebSocket;
			cout << u8"ws�ͻ���׼������...�ȴ�������..." << endl;
			std::unique_ptr<WebSocket> ws(WebSocket::from_url("ws://localhost:6700/"));
			if (ws == false) {
				printf(u8"�������ӵ�websocket������ʧ�ܡ�");
				return;
			}
			cout << u8"���ӳɹ������ڼ���..." << endl;
			if (DEBUGMODE) {
				cout << u8"Debugģʽ������..." << endl;
			}
			while (true) {
				WebSocket::pointer wsp = &*ws;
				ws->poll();
				ws->dispatch([wsp](const std::string& message) {
					if (DEBUGMODE) {
						cout << message << "\n" << endl;
					}
					if (_stricmp(message.substr(0, 6).c_str(), "{\"data") != 0) { //���Ի�ִ��Ϣ
						json j = json::parse(message);
						/*
						Message�¼�����
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
								sprintf_s(msg, u8"[%s] [osucat]: �յ����Ժ��� %lld ����Ϣ��%s", utils::unixTime2Str(tar.time).c_str(), tar.user_id, tar.message.c_str());
								cout << msg << endl;
								if (tar.message[0] == '!' || tar.message.find(u8"��") == 0) {
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
								sprintf_s(msg, u8"[%s] [osucat]: �յ�����Ⱥ %lld �� %lld ����Ϣ��%s", utils::unixTime2Str(tar.time).c_str(), tar.group_id, tar.user_id, tar.message.c_str());
								cout << msg << endl;
								if (tar.message[0] == '!' || tar.message.find(u8"��") == 0) {
									string str = tar.message;
									str = tar.message[0] < 0 ? tar.message.substr(3) : tar.message.substr(1);
									string params = { 0 };
									if (cmdParse(str, tar, &params)) {
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
						Request�¼�����
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
								sprintf_s(msg, u8"[%s] [osucat]: �յ������û� %lld �ĺ�������%s\n", utils::unixTime2Str(req.time).c_str(), req.user_id, req.message.c_str());
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
									//ֻ��������Ⱥ�Żᱻ����
									char msg[4096];
									sprintf_s(msg, u8"[%s] [osucat]: �յ������û� %lld ��Ⱥ����������%s\n", utils::unixTime2Str(req.time).c_str(), req.user_id, req.message.c_str());
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
			return false;
		}
		//done
		static void help(string cmd, string* params) {
			utils::trim(cmd);
			utils::string_replace(cmd, " ", "");
			//string dir = (string)OC_ROOT_PATH + "\\", fileStr;
			if (_stricmp(cmd.substr(0, 4).c_str(), "bind") == 0) {
				*params = u8"[CQ:image,file=osucat\\help\\��.png]";
				return;
			}
			if (_stricmp(cmd.substr(0, 4).c_str(), "info") == 0) {
				*params = u8"[CQ:image,file=osucat\\help\\������Ϣ.png]";
				return;
			}
			if (_stricmp(cmd.substr(0, 5).c_str(), "score") == 0) {
				*params = u8"[CQ:image,file=osucat\\help\\�ɼ�.png]";
				return;
			}
			if (_stricmp(cmd.substr(0, 6).c_str(), "custom") == 0) {
				*params = u8"[CQ:image,file=osucat\\help\\�Զ���.png]";
				return;
			}
			if (_stricmp(cmd.substr(0, 5).c_str(), "other") == 0) {
				*params = u8"[CQ:image,file=osucat\\help\\����.png]";
				return;
			}
			*params = u8"[CQ:image,file=osucat\\help\\����.png]";
			return;
		}
		static void about(string* params) {
			*params = u8"[CQ:image,file=osucat\\help\\about.png]";
		}
		static void contact(string cmd, Target tar, string* params) {
			utils::trim(cmd);
			*params = u8"[CQ:at,qq=" + to_string(tar.user_id) + u8"] ���봫��Ļ��ѳɹ������������Ŷ��";
			Target activepushTar;
			activepushTar.message_type = Target::MessageType::PRIVATE;
			activepushTar.user_id = 451577581;
			activepushTar.message = u8"�յ������û� " + to_string(tar.user_id) + u8" ����Ϣ��" + cmd;
			activepush(activepushTar);
		}
	private:
		/*
		ͨ��message_type���ж���Ⱥ����Ϣ���Ǻ�����Ϣ
		message�������ͬ��params
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