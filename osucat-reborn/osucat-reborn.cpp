#include "osucat-reborn.h"

using namespace std;
static unsigned old_code_page;

char wshost[64];
int wsport;

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
	cout << "Done!\nCurrent Directory Path: \"" << OC_ROOT_PATH << "\"" << endl;
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
		ifstream configfile(".\\osucat_config.json");
		string config;
		if (configfile)
		{
			json j;
			try {
				ostringstream tmp;
				tmp << configfile.rdbuf();
				config = tmp.str();
				configfile.close();
				j = json::parse(config);
			}
			catch (std::exception& ex) {
				cout << ex.what() << endl;
				system("pause");
				return 0;
			}
			if (j["apikey"].get<string>() == "") { cout << "\nMissing API key!\n" << endl; system("pause"); return 0; }
			else sprintf_s(OSU_KEY, "%s", j["apikey"].get<string>().c_str());
			if (j["debugmode"].get<bool>()) { DEBUGMODE = true; cout << u8"Debug mode is enabled...\n" << endl; }
			else DEBUGMODE = false;
			if (j["use_remote_server"].get<bool>() == false) {
				if (!utils::fileExist(".\\go-cqhttp.exe")) {
					cout << "Missing go-cqhttp file." << endl;
					cout << "Get from https://github.com/Mrs4s/go-cqhttp/releases" << endl;
					system("pause");
					return 0;
				}
				if (j["local_settings"]["ws_host"].get<string>() != "") {
					cout << "\n\nMissing \"ws_host\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else sprintf_s(wshost, "%s", j["local_settings"]["ws_host"].get<string>().c_str());
				if (j["local_settings"]["ws_port"].is_null()) {
					cout << "\n\nMissing \"ws_port\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else wsport = j["local_settings"]["ws_port"].get<int>();
				if (j["local_settings"]["sql_user"].get<string>() != "") {
					cout << "\n\nMissing \"sql_user\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else sprintf_s(SQL_USER, "%s", j["local_settings"]["sql_user"].get<string>().c_str());
				if (j["local_settings"]["sql_host"].get<string>() != "") {
					cout << "\n\nMissing \"sql_host\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else sprintf_s(SQL_HOST, "%s", j["local_settings"]["sql_host"].get<string>().c_str());
				if (j["local_settings"]["sql_password"].get<string>() != "") {
					cout << "\n\nMissing \"sql_password\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else sprintf_s(SQL_PWD, "%s", j["local_settings"]["sql_password"].get<string>().c_str());
				if (j["local_settings"]["sql_database"].get<string>() != "") {
					cout << "\n\nMissing \"sql_database\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else sprintf_s(SQL_DATABASE, "%s", j["local_settings"]["sql_database"].get<string>().c_str());
				if (j["local_settings"]["sql_port"].is_null()) {
					cout << "\n\nMissing \"sql_port\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else SQL_PORT = j["local_settings"]["sql_port"].get<int>();
			}
			else {
				cout << u8"\n/*** osucat now using remote server ***/\n" << endl;
				if (j["remote_settings"]["ws_host"].get<string>() != "") {
					cout << "\n\nMissing \"ws_host\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else sprintf_s(wshost, "%s", j["remote_settings"]["ws_host"].get<string>().c_str());
				if (j["remote_settings"]["ws_port"].is_null()) {
					cout << "\n\nMissing \"ws_port\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else wsport = j["remote_settings"]["ws_port"].get<int>();
				if (j["remote_settings"]["sql_user"].get<string>() != "") {
					cout << "\n\nMissing \"sql_user\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else sprintf_s(SQL_USER, "%s", j["remote_settings"]["sql_user"].get<string>().c_str());
				if (j["remote_settings"]["sql_host"].get<string>() != "") {
					cout << "\n\nMissing \"sql_host\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else sprintf_s(SQL_HOST, "%s", j["remote_settings"]["sql_host"].get<string>().c_str());
				if (j["remote_settings"]["sql_password"].get<string>() != "") {
					cout << "\n\nMissing \"sql_password\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else sprintf_s(SQL_PWD, "%s", j["remote_settings"]["sql_password"].get<string>().c_str());
				if (j["remote_settings"]["sql_database"].get<string>() != "") {
					cout << "\n\nMissing \"sql_database\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else sprintf_s(SQL_DATABASE, "%s", j["remote_settings"]["sql_database"].get<string>().c_str());
				if (j["remote_settings"]["sql_port"].is_null()) {
					cout << "\n\nMissing \"sql_port\" settings..\n\n\n" << endl;
					system("pause");
					return 0;
				}
				else SQL_PORT = j["remote_settings"]["sql_port"].get<int>();
			}
		}
		else
		{
			cout << "Missing config file.\nInitializing config file..." << endl;
			ofstream writeconfig(".\\osucat_config.json");
			if (!writeconfig) {
				cout << "Unable to create config file.";
				system("pause");
				return 0;
			}
			json j;
			j["apikey"] = "";
			j["use_remote_server"] = false;
			j["debugmode"] = false;
			j["local_settings"]["ws_host"] = "127.0.0.1";
			j["local_settings"]["ws_port"] = 6700;
			j["local_settings"]["sql_host"] = "127.0.0.1";
			j["local_settings"]["sql_user"] = "";
			j["local_settings"]["sql_password"] = "";
			j["local_settings"]["sql_database"] = "";
			j["local_settings"]["sql_port"] = 3306;
			j["remote_settings"]["ws_host"] = "";
			j["remote_settings"]["ws_port"] = 6700;
			j["remote_settings"]["sql_host"] = "";
			j["remote_settings"]["sql_user"] = "";
			j["remote_settings"]["sql_password"] = "";
			j["remote_settings"]["sql_database"] = "";
			j["remote_settings"]["sql_port"] = 3306;
			writeconfig << j.dump().c_str() << endl;
			writeconfig.close();
			cout << "\n\nThe config file has created. Please complete the settings.\n\n\n";
			system("pause");
			return 0;
		}
	}
	else {
		ISACTIVE = false;
		cout << u8"\nosucat is not activated,\nthe program is about to exit..." << endl;;
	}
	if (ISACTIVE) {
		cout << u8"Creating daily update thread..." << endl;
		osucat::main::_CreateDUThread();
		cout << "Loading the admin list..." << endl;
		try {
			Database db;
			db.Connect();
			if (db.reloadAdmin()) {
				cout << "The admin list has successfully loaded!" << endl;
			}
			else {
				cout << "Failed. Please check database settings." << endl;
			}
		}
		catch (osucat::database_exception) {
			cout << "\n\nUnable to connect to the database. Is the database settings correctly filled in?\n\n\n" << endl;
			system("pause");
			return 0;
		}
		INT rc;
		WSADATA wsaData;
		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) {
			printf("WSAStartup Failed.\n");
			system("pause");
			return 1;
		}
		using easywsclient::WebSocket;
		cout << u8"Ready...Waiting for connection..." << endl;
		char tmp[1024];
		sprintf_s(tmp, "ws://%s:%d/", wshost, wsport);
		std::unique_ptr<WebSocket> ws(WebSocket::from_url(tmp));
		if (ws == false) {
			printf(u8"Trying to connect to the WebSocket server has failed.");
			system("pause");
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
		system("pause");
	}
	return 0;
}
