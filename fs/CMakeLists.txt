set(
        MINI_FS_SERVER_UTILS_SOURCES
        module/server_utils/disk_utils.c
)

set(
        MINI_FS_HANDLER_SOURCES
        module/handlers/create_dir.c
        module/handlers/init_fs.c
        module/handlers/destroy_fs.c
        module/handlers/list_dir.c
        module/handlers/delete_object.c
        module/handlers/create_file.c
        module/handlers/open_file.c
        module/handlers/close_file.c
        module/handlers/seek_pos.c
        module/handlers/write_to_file.c
        module/handlers/read_from_file.c
)

set(
        MINI_FS_STRUCTURES_SOURCES
        module/structures/inode.c
        module/structures/superblock.c
        module/structures/dir_record.c
        module/structures/descriptor_table.c
)

set(
        MINI_FS_NET_SOURCES
        net/loop.c
)

add_executable(
        mini_fs_server
        main.c
        ${MINI_FS_HANDLER_SOURCES}
        ${MINI_FS_STRUCTURES_SOURCES}
        ${MINI_FS_NET_SOURCES}
        ${MINI_FS_SERVER_UTILS_SOURCES}
)
