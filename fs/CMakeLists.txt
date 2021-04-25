add_subdirectory(module)

set(
        MINI_FS_NET_SOURCES
        net/loop.c
)

add_executable(
        mini_fs_server
        main.c
        ${MINI_FS_NET_SOURCES}
)
