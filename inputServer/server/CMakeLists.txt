# server/CMakeLists.txt

add_executable(server
    server.c
    serverCommands.c
    serverRewrite.c
    client_handler.c
)

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../util
)

target_link_libraries(server PRIVATE util pthread)
