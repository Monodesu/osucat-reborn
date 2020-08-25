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

	osucat::main::monitors();

	WSACleanup();
	cout << u8"ws连接已关闭。" << endl;
	return 0;
}
