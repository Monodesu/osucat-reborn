// osucat-httpapi.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
char OC_ROOT_PATH[512];
bool DEBUGMODE = false;
bool ISACTIVE = false;
long long int owner_userid;
long long int management_groupid;

#include <easywsclient/easywsclient.hpp>
#include <json/json.hpp>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <string>
#include <cmath>
#include <memory>
#include <time.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <set>
#include <schannel.h>
#include <winhttp.h>
#include <fstream>
#include <regex>
#include <thread>
#include <codecvt>
#include <optional>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winhttp.lib")

#include "utils.hpp"
#include "api.hpp"
#include "osucat_exception.hpp"
#include "HTTP.hpp"
#include "osu_api_v1.hpp"
#include "badge.hpp"
#include "mysql.hpp"
#include "oppai-cpp/oppai.hpp"
#include "imageMaker.hpp"
#include "osucat-main.hpp"

// TODO: 在此处引用程序需要的其他标头。
