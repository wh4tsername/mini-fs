set(
        MINI_FS_HANDLER_SOURCES
        handlers/create_dir.c
        handlers/init_fs.c
        handlers/destroy_fs.c
        handlers/list_dir.c
        handlers/delete_object.c
        handlers/create_file.c
        handlers/open_file.c
        handlers/close_file.c
        handlers/seek_pos.c
        handlers/write_to_file.c
        handlers/read_from_file.c
)

set(
        MINI_FS_STRUCTURES_SOURCES
        structures/inode.c
        structures/superblock.c
        structures/dir_record.c
        structures/descriptor_table.c
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
)
