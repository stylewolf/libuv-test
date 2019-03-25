project(uc-pipe-base)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
include_directories(
        include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/libuv/include/${PLATFORM}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/jsonc/include/${PLATFORM}
)
link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/libuv/lib/${PLATFORM}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/jsonc/lib/${PLATFORM}
)
link_libraries(
        uv.a
        json-c.a
)
add_library(${PROJECT_NAME} ${DIR_SRCS})
target_link_libraries(${PROJECT_NAME} uc-protocol-json dl pthread rt)