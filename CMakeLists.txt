set(CMAKE_SYSTEM_NAME Linux)
cmake_minimum_required(VERSION 3.13)
project(engine_tools)
#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 11)

set(PLATFORM arm)
set(TP third-party)
#add_definitions("-Werror -Wall -g")
add_definitions("-Wall -g")
# 加入一个配置头文件，用于处理 CMake 对源码的设置
configure_file(
        "${PROJECT_SOURCE_DIR}/config.h.in"
        "${PROJECT_SOURCE_DIR}/config.h"
)
# Add the source in project root directory
aux_source_directory(. DIRSRCS)

#pipe
include_directories(./ pipe/base pipe/server pipe/client)
add_subdirectory(pipe/base)
add_subdirectory(pipe/server)
add_subdirectory(pipe/client)
#add_subdirectory(pipe/client)
##sqlite
#include_directories(sqlite/server sqlite/client)
#add_subdirectory(sqlite/server)
#add_subdirectory(sqlite/client)
##network
#include_directories(network/udp)
#add_subdirectory(network/udp)
#protocol
include_directories(protocol/json)
add_subdirectory(protocol/json)
#libs
include_directories(utils/sqlite3 utils/xmalloc)
add_subdirectory(utils/sqlite3)
add_subdirectory(utils/xmalloc)
##test
include_directories(test/pipe/server test/pipe/client)
add_subdirectory(test/pipe/server)
add_subdirectory(test/pipe/client)
##add_subdirectory(test/test-pipe-server)
##add_subdirectory(test/test-pipe-client)
##add_subdirectory(test/test-pipe-client1)
#add_subdirectory(test/test-db-server)
#add_subdirectory(test/test-db-client0)
#add_subdirectory(test/test-udp-engine)