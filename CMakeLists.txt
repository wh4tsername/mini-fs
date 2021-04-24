cmake_minimum_required(VERSION 3.17)
project(mini_fs C)

set(CMAKE_C_STANDARD 11)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")

add_subdirectory(utils)

add_subdirectory(fs)

target_link_libraries(mini_fs_server PRIVATE utils)

add_subdirectory(client)

target_link_libraries(mini_fs_client PRIVATE utils)
