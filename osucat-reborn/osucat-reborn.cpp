#include "osucat-reborn.h"

using namespace std;
static unsigned old_code_page;

int main()
{
	// 保存当前控制台代码页
	old_code_page = GetConsoleCP();
	// 设置控制台代码页为 UTF-8
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	std::vector<std::string> consoleWelcome;
	consoleWelcome.push_back("-----------------------------------------------------------------------------------");
	consoleWelcome.push_back("                                                                                   ");
	consoleWelcome.push_back(R"(     _____         ___             ___         ___           _____                )");
	consoleWelcome.push_back(R"(    /  /::\       /  /\           /__/\       /  /\         /  /::\        ___    )"); 
	consoleWelcome.push_back(R"(   /  /:/\:\     /  /:/ __        \  \:\     /  /:/        /  /:  :\      /  /\   )"); 
	consoleWelcome.push_back(R"(  /  /:/  \:\   /  /:/ /::\   ___  \  \:\   /  /:/  ___   /  /:/~/::\    /  /:/   )"); 
	consoleWelcome.push_back(R"( /__/:/ \__\:\ /__/:/ /:/\:\ /__/\  \__\:\ /__/:/  /  /\ /__/:/ /:/\:\  /  /::\   )"); 
	consoleWelcome.push_back(R"( \  \:\ /  /:/ \  \:\/:/~/:/ \  \:\ /  /:/ \  \:\ /  /:/ \  \:\/:/__\/ /__/:/\:\  )");
	consoleWelcome.push_back(R"(  \  \:\  /:/   \  \::/ /:/   \  \:\  /:/   \  \:\  /:/   \  \::/      \__\/  \:\ )");
	consoleWelcome.push_back(R"(   \  \:\/:/     \__\/ /:/     \  \:\/:/     \  \:\/:/     \  \:\           \  \:\)");
	consoleWelcome.push_back(R"(    \  \::/        /__/:/       \  \::/       \  \::/       \  \:\           \__\/)");
	consoleWelcome.push_back(R"(     \__\/         \__\/         \__\/         \__\/         \__\/                )");
	consoleWelcome.push_back("                                                                                   ");
	consoleWelcome.push_back("-----------------------------------------------------------------------------------");
	consoleWelcome.push_back("Initializing...");
	for (int i = 0; i < consoleWelcome.size() ;++i) {
		cout << consoleWelcome[i].c_str() << endl;
	}
	GetCurrentDirectory(512, OC_ROOT_PATH);
	SetCurrentDirectory(OC_ROOT_PATH);
	Magick::InitializeMagick(OC_ROOT_PATH);
	cout << "Done!\n" << endl;
	cout << "Current Directory Path: \"" << OC_ROOT_PATH << "\"" << endl;
	if (utils::fileExist("./.active")) {
		ISACTIVE = true;
		cout << u8"\nosucat已激活\n" << endl;;
	}
	else {
		ISACTIVE = false;
		cout << u8"\nosucat未启用\n程序即将退出.." << endl;;
	}
	if (ISACTIVE) {
		utils::fileExist("./.debug") ? DEBUGMODE = true : DEBUGMODE = false; //判断是否启用了debug模式
		if (DEBUGMODE) cout << u8"Debug模式已启用...\n" << endl;
		INT rc;
		WSADATA wsaData;
		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) {
			printf("WSAStartup Failed.\n");
			return 1;
		}
		using easywsclient::WebSocket;
		cout << u8"准备就绪...等待连接中..." << endl;
		std::unique_ptr<WebSocket> ws(WebSocket::from_url("ws://localhost:6700/"));
		if (ws == false) {
			printf(u8"尝试连接到websocket服务器失败。");
			return 2;
		}
		cout << u8"连接成功，正在监听..." << endl;
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
	}
	system("pause");
	return 0;
}
