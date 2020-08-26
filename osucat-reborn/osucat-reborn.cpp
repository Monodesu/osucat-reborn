#include "osucat-reborn.h"

using namespace std;
static unsigned old_code_page;

int main()
{
	GetCurrentDirectory(512, OC_ROOT_PATH);
	cout << "Current Directory Path: \"" << OC_ROOT_PATH << "\"" << endl;
	SetCurrentDirectory(OC_ROOT_PATH);
	Magick::InitializeMagick(OC_ROOT_PATH);
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
	if (ws == false) {
		printf(u8"尝试连接到websocket服务器失败。");
		return 2;
	}
	cout << u8"连接成功，正在监听..." << endl;
	if (DEBUGMODE) cout << u8"Debug模式已启用..." << endl;
	while (true) {
		WebSocket::pointer wsp = &*ws;
		ws->poll();
		ws->dispatch([wsp](const std::string& message) {
			if (DEBUGMODE) cout << message << "\n" << endl;
			thread osucatMultiThread(bind(&osucat::main::osucatMain, message));
			osucatMultiThread.detach(); //wsp->close();
			});
	}
	WSACleanup();
	cout << u8"ws连接已丢失。" << endl;
	system("pause");
	return 0;
}
