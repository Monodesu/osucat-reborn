# CMakeList.txt: osucat-reborn 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

include_directories(include) # 设置 src 目录为包含目录, 可根据实际情况修改
include_directories(extern/Mysql/include)
include_directories(extern/Magick++/include)

#file(GLOB_RECURSE SOURCE_FILES src/*.cpp) # 递归搜索 src 目录中的源文件, 可根据实际情况修改
# 将源代码添加到此项目的可执行文件。
add_executable (osucat-reborn "osucat-reborn.cpp" "osucat-reborn.h" "include/easywsclient/easywsclient.hpp" "include/easywsclient/easywsclient.cpp" "api.hpp" "include/json/json.hpp"  "utils.hpp" "osucat-main.hpp" "HTTP.hpp" "osucat_exception.hpp" "osu_api_v1.hpp"  "badge.hpp" "mysql.hpp" "imageMaker.hpp" "include/oppai-cpp/oppai/mods.hpp" "include/oppai-cpp/oppai/oppai.c" "include/oppai-cpp/oppai.hpp" "message.h"  "osucat-addons.hpp")

target_link_libraries(osucat-reborn
        "${CMAKE_CURRENT_SOURCE_DIR}\\extern\\Mysql\\lib\\libmysql.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}\\extern\\Mysql\\lib\\mysqlclient.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}\\extern\\Magick++\\lib\\CORE_RL_Magick++_.lib"
)
# TODO: 如有需要，请添加测试并安装目标。
