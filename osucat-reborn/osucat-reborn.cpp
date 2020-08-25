
#include "osucat-reborn.h"



//#include <assert.h> //断言


using namespace std;
using json = nlohmann::json;
static unsigned old_code_page;

int main()
{
	// 保存当前控制台代码页
	old_code_page = GetConsoleCP();
	// 设置控制台代码页为 UTF-8
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	INT rc;
	WSADATA wsaData;

	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rc) {
		printf("WSAStartup Failed.\n");
		return 1;
	}

	using easywsclient::WebSocket;


	cout << u8"ws客户端准备就绪...等待连接中..." << endl;
	std::unique_ptr<WebSocket> ws(WebSocket::from_url("ws://localhost:6700/"));


	//assert(ws);
	if (ws == false) {
		printf(u8"尝试连接到websocket服务器失败。");
		return 0;
	}



	cout << u8"连接成功，正在监听..." << endl;
	if (DEBUGMODE) {
		cout << u8"Debug模式已启用..." << endl;
	}
	while (true) {

		WebSocket::pointer wsp = &*ws;
		ws->poll();
		ws->dispatch([wsp](const std::string& message) {
			//char buffer[8192];
			//sprintf_s(buffer, "%s", message.c_str());
			//string rawStr = message;
			if (DEBUGMODE) {
				cout << message << "\n" << endl;
			}
			if (_stricmp(message.substr(0, 6).c_str(), "{\"data") != 0) { //忽略回执消息



				json j = json::parse(message);
				json sj = json::parse(message)["sender"];
				if (j["post_type"].get<string>() == "message") {
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
						sprintf_s(msg, u8"[%s] [osucat]: 收到来自好友 %lld 的消息：%s", osucat::OCUtils::unixTime2Str(tar.time).c_str(), tar.user_id, tar.message.c_str());
						json msg;
						msg["action"] = "send_msg";
						msg["params"] = {
							{"message_type","private"},
							{"user_id",tar.user_id} ,
							{"message",u8"你发送了一条消息，这是来自osucat的已读回执。"}
						};
						msg["echo"] = "success";
						wsp->send(msg.dump());
					}
					if (tar.message_type == Target::MessageType::GROUP) {
						sprintf_s(msg, u8"[%s] [osucat]: 收到来自群 %lld 的 %lld 的消息：%s", osucat::OCUtils::unixTime2Str(tar.time).c_str(), tar.group_id, tar.user_id, tar.message.c_str());
						if (tar.message.find("[CQ:at,qq=834276213]") != string::npos) {
							json msg;
							msg["action"] = "send_msg";
							msg["params"] = {
								{"message_type","group"},
								{"group_id",tar.group_id} ,
								{"message",u8"[CQ:at,qq=" + to_string(tar.user_id) + u8"] 你@了osucat，这是来自osucat的已读回执。"}
							};
							msg["echo"] = "success";
							wsp->send(msg.dump());
						}
						if (tar.message.find(u8"osucat图片测试") != string::npos) {
							json msg;
							msg["action"] = "send_msg";
							msg["params"] = {
								{"message_type","group"},
								{"group_id",tar.group_id},
							    {"message","[CQ:image,file=https://s1.ax1x.com/2020/08/25/d6i1eA.png]"}
							};
							msg["echo"] = "success";
							wsp->send(msg.dump());
						}
					}


					cout << msg << endl;
				}

				if (j["post_type"].get<string>() == "request") {
					Request req;
					req.message = j["comment"].get<string>();
					req.user_id = j["user_id"].get<int64_t>();
					req.flag = j["flag"].get<int64_t>();
					req.time = j["time"].get<int64_t>();
					char msg[2048];
					sprintf_s(msg, u8"[%s] [osucat]: 收到来自用户 %lld 的好友请求：%s\n", osucat::OCUtils::unixTime2Str(req.time).c_str(), req.user_id, req.message.c_str());
					cout << msg << endl;
				}

			}



			//if (message == "world") { wsp->close(); }
			});
	}
	WSACleanup();
	cout << u8"ws连接已关闭。" << endl;
	return 0;
}
