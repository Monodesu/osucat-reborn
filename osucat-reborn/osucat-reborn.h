// osucat-httpapi.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
char OC_ROOT_PATH[512];
constexpr auto DEBUGMODE = false;

#include <easywsclient/easywsclient.hpp>
#include <json/json.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <memory>
#include <time.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <schannel.h>
#include <winhttp.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <codecvt>

#pragma comment( lib, "ws2_32" )
#pragma comment(lib, "winhttp.lib")

#include "utils.hpp"
#include "api.hpp"
#include "osucat_exception.hpp"
#include "HTTP.hpp"
// TODO: 在此处引用程序需要的其他标头。
