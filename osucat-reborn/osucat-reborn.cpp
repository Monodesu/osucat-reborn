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
	for (int i = 0; i < consoleWelcome.size(); ++i) {
		cout << consoleWelcome[i].c_str() << endl;
	}
	GetCurrentDirectory(512, OC_ROOT_PATH);
	SetCurrentDirectory(OC_ROOT_PATH);
	Magick::InitializeMagick(OC_ROOT_PATH);
	cout << "Done!" << endl;
	cout << "Current Directory Path: \"" << OC_ROOT_PATH << "\"" << endl;
#pragma region DetecedFolder
	if (!utils::isDirExist(".\\data")) {
		cout << "Folder 'data' does not exist, created." << endl;
		CreateDirectoryA(".\\data", NULL);
	}
	if (!utils::isDirExist(".\\data\\cache")) {
		cout << "Folder 'data\\cache' does not exist, created." << endl;
		CreateDirectoryA(".\\data\\cache", NULL);
	}
	if (!utils::isDirExist(".\\data\\images")) {
		cout << "Folder 'data\\images' does not exist, created." << endl;
		CreateDirectoryA(".\\data\\images", NULL);
	}
	if (!utils::isDirExist(".\\data\\images\\osucat")) {
		cout << "Folder 'data\\images\\osucat' does not exist, created." << endl;
		CreateDirectoryA(".\\data\\images\\osucat", NULL);
	}
	if (!utils::isDirExist(".\\data\\images\\osucat\\help")) {
		cout << "Folder 'data\\images\\osucat\\help' does not exist, created." << endl;
		CreateDirectoryA(".\\data\\images\\osucat\\help", NULL);
	}
	if (!utils::isDirExist(".\\data\\images\\osucat\\custom")) {
		cout << "Folder 'data\\images\\osucat\\custom' does not exist, created." << endl;
		CreateDirectoryA(".\\data\\images\\osucat\\custom", NULL);
	}
	if (!utils::isDirExist(".\\data\\images\\osucat\\custom\\banner_verify")) {
		cout << "Folder 'data\\images\\osucat\\banner_verify' does not exist, created." << endl;
		CreateDirectoryA(".\\data\\images\\osucat\\custom\\banner_verify", NULL);
	}
	if (!utils::isDirExist(".\\data\\images\\osucat\\custom\\infopanel_verify")) {
		cout << "Folder 'data\\images\\osucat\\infopanel_verify' does not exist, created." << endl;
		CreateDirectoryA(".\\data\\images\\osucat\\custom\\infopanel_verify", NULL);
	}
	if (!utils::isDirExist(".\\work")) {
		cout << "Folder 'work' does not exist, created." << endl;
		CreateDirectoryA(".\\work", NULL);
	}
	if (!utils::isDirExist(".\\work\\avatar")) {
		cout << "Folder 'work\\avatar' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\avatar", NULL);
	}
	if (!utils::isDirExist(".\\work\\background")) {
		cout << "Folder 'work\\background' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\background", NULL);
	}
	if (!utils::isDirExist(".\\work\\badges")) {
		cout << "Folder 'work\\badges' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\badges", NULL);
	}
	if (!utils::isDirExist(".\\work\\flags")) {
		cout << "Folder 'work\\flags' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\flags", NULL);
	}
	if (!utils::isDirExist(".\\work\\fonts")) {
		cout << "Folder 'fonts' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\fonts", NULL);
	}
	if (!utils::isDirExist(".\\work\\icons")) {
		cout << "Folder 'work\\icon' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\icons", NULL);
	}
	if (!utils::isDirExist(".\\work\\mode_icon")) {
		cout << "Folder 'work\\mode_icon' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\mode_icon", NULL);
	}
	if (!utils::isDirExist(".\\work\\mods")) {
		cout << "Folder 'work\\mods' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\mods", NULL);
	}
	if (!utils::isDirExist(".\\work\\ranking")) {
		cout << "Folder 'work\\ranking' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\ranking", NULL);
	}
	if (!utils::isDirExist(".\\work\\v1_cover")) {
		cout << "Folder 'work\\v1_cover' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\v1_cover", NULL);
	}
	if (!utils::isDirExist(".\\work\\v1_infopanel")) {
		cout << "Folder 'work\\v1_infopanel' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\v1_infopanel", NULL);
	}
	if (!utils::isDirExist(".\\work\\v2_background")) {
		cout << "Folder 'work\\v2_background' does not exist, created." << endl;
		CreateDirectoryA(".\\work\\v2_background", NULL);
	}
#pragma endregion
	if (utils::fileExist(".\\.active")) {
		ISACTIVE = true;
		cout << u8"osucat is activated!" << endl;;
		if (utils::fileExist(".\\.remote")) {
			cout << u8"\n/*** osucat now using remote sql server ***/\n" << endl;
			sprintf_s(SQL_USER, "root");
			sprintf_s(SQL_HOST, "192.168.0.103");
			sprintf_s(SQL_PWD, "ASDasdASD32111!");
			sprintf_s(SQL_DATABASE, "osucat");
			SQL_PORT = 32148;
		}
		else {
			sprintf_s(SQL_USER, "root");
			sprintf_s(SQL_HOST, "127.0.0.1");
			sprintf_s(SQL_PWD, "ASDasdASD32111!");
			sprintf_s(SQL_DATABASE, "osucat");
			SQL_PORT = 32148;
		}
	}
	else {
		ISACTIVE = false;
		cout << u8"\nosucat is not activated,\nthe program is about to exit..." << endl;;
	}
	if (ISACTIVE) {
		utils::fileExist(".\\.debug") ? DEBUGMODE = true : DEBUGMODE = false; //判断是否启用了debug模式
		if (DEBUGMODE) cout << u8"Debug mode is enabled...\n" << endl;
		cout << u8"Creating daily update thread..." << endl;
		osucat::main::_CreateDUThread();

		INT rc;
		WSADATA wsaData;
		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) {
			printf("WSAStartup Failed.\n");
			return 1;
		}
		using easywsclient::WebSocket;
		cout << u8"Ready...Waiting for connection..." << endl; 
		std::unique_ptr<WebSocket> ws(WebSocket::from_url(utils::fileExist(".\\.remote")?"ws://192.168.0.103:6700":"ws://localhost:6700/"));
		if (ws == false) {
			printf(u8"An attempt to connect to the WebSocket server has failed.");
			return 2;
		}
		cout << u8"WebSocket connection was successfully established!" << endl;
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
		cout << u8"WebSocket connection closed." << endl;
	}
	return 0;
}
