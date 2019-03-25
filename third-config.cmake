include_directories(
        include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/libuv/include/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/jsonc/include/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/sqlite3/include/${PLATFORM}
)
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/libuv/lib/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/jsonc/lib/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../${TP}/sqlite3/lib/${PLATFORM}
)

link_libraries(
        uv.a
        json-c.a
        sqlite3.a
)