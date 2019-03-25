project(uc-protocol-json)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
include_directories(
        include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/jsonc/include/${PLATFORM}
)
link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/jsonc/lib/${PLATFORM}
)
link_libraries(
        json-c.a
)
add_library(${PROJECT_NAME} ${DIR_SRCS})
target_link_libraries(${PROJECT_NAME} dl pthread rt)