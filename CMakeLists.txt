cmake_minimum_required(VERSION 3.17)
project(mini_fs C)

set(CMAKE_C_STANDARD 11)

set(MINI_FS_CLIENT_SOURCES client/client.c)
set(
        MINI_FS_HANDLER_SOURCES
        fs/handlers/create_dir.c
        fs/handlers/init_fs.c
        fs/handlers/destroy_fs.c
        fs/handlers/list_dir.c
        fs/handlers/delete_object.c
        fs/handlers/create_file.c fs/handlers/open_file.c fs/handlers/close_file.c fs/handlers/seek_pos.c fs/handlers/write_to_file.c fs/handlers/read_from_file.c)
set(MINI_FS_STRUCTURES_SOURCES fs/inode.c fs/superblock.c fs/dir_record.c fs/descriptor_table.c)

add_subdirectory(utils)

add_executable(
        mini_fs
        main.c
        ${MINI_FS_CLIENT_SOURCES}
        ${MINI_FS_HANDLER_SOURCES}
        ${MINI_FS_STRUCTURES_SOURCES}
)

target_link_libraries(mini_fs PRIVATE utils)
