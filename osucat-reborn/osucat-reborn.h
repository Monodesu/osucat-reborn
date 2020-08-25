// osucat-httpapi.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
char OC_ROOT_PATH[512];
constexpr auto DEBUGMODE = true;

#include <easywsclient/easywsclient.hpp>
#include <json/json.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <memory>
#include <time.h>
#include <stdlib.h>
#include <WinSock2.h>

#pragma comment( lib, "ws2_32" )

#include "api.hpp"
#include "utils.hpp"
// TODO: 在此处引用程序需要的其他标头。
