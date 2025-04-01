project (tcp_client)
add_executable (tcp_client process_watcher.cpp)
target_link_libraries(tcp_client PUBLIC pthread)