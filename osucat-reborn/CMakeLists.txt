# CMakeList.txt: osucat-httpapi 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

include_directories(include) # 设置 src 目录为包含目录, 可根据实际情况修改


# 将源代码添加到此项目的可执行文件。
add_executable (osucat-httpapi "osucat-reborn.cpp" "osucat-reborn.h" "include/easywsclient/easywsclient.hpp" "include/easywsclient/easywsclient.cpp" "api.hpp" "include/json/json.hpp"  "utils.hpp")

# TODO: 如有需要，请添加测试并安装目标。
