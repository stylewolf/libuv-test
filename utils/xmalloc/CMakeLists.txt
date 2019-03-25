project(uc-utils-xmalloc)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

add_library(${PROJECT_NAME} ${DIR_SRCS} include/xmalloc.h xmalloc.cpp)
target_link_libraries(${PROJECT_NAME} dl pthread rt)