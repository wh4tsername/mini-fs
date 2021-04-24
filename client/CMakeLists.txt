set(
        MINI_FS_CLIENT_SOURCES
        main.c
        client/client.c
        client/net_handlers.c
        client/connection.c
)

add_executable(
        mini_fs_client
        ${MINI_FS_CLIENT_SOURCES}
)
