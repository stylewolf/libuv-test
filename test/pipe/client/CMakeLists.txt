project(test-pipe-client)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

include(../../../test-config.cmake)
add_executable(${PROJECT_NAME} ${DIR_SRCS} )
target_link_libraries(${PROJECT_NAME} uc-pipe-client  dl pthread rt)