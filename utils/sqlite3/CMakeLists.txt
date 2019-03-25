project(uc-utils-sqlite3)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
include_directories(
        include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/sqlite3/include/${PLATFORM}
)
link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/sqlite3/lib/${PLATFORM}
)
link_libraries(
        sqlite3.a
)
add_library(${PROJECT_NAME} ${DIR_SRCS} include/sqlite3ppext.h include/sqlite3pp.h sqlite3ppext.cpp sqlite3pp.cpp)
target_link_libraries(${PROJECT_NAME} dl pthread rt)