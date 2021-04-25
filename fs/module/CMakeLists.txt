set(
        MINI_FS_SERVER_UTILS_SOURCES
        server_utils/disk_utils.c
        server_utils/helpers.c
)

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
        structures/fs_inode.c
        structures/superblock.c
        structures/dir_record.c
        structures/descriptor_table.c
)

set(KERNEL_DIR "" CACHE STRING "Path to the kernel build directory")

if("${KERNEL_DIR}" STREQUAL "")
    execute_process(COMMAND uname -r OUTPUT_VARIABLE uname_r
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(KERNEL_DIR "/lib/modules/${uname_r}/build")
endif()

find_file(KERNEL_MAKEFILE NAMES Makefile PATHS ${KERNEL_DIR} NO_DEFAULT_PATH)

if(NOT KERNEL_MAKEFILE)
    message(FATAL_ERROR "There is no Makefile in kerneldir!")
endif()

add_library(
        mini-fs-module-lib
        STATIC
        ${MINI_FS_SERVER_UTILS_SOURCES}
        ${MINI_FS_HANDLER_SOURCES}
        ${MINI_FS_STRUCTURES_SOURCES}
)
get_target_property(MINI_FS_MODULE_SOURCES mini-fs-module-lib SOURCES)
list(APPEND MINI_FS_MODULE_SOURCES mini-fs-module.c)

string(REPLACE ";" " " MINI_FS_MODULE_SOURCES_STRING "${MINI_FS_MODULE_SOURCES}")
configure_file(Kbuild.in Kbuild @ONLY)

foreach(src ${MINI_FS_MODULE_SOURCES})
    configure_file(${src} ${src} COPYONLY)
endforeach()

set(build_module ${CMAKE_MAKE_PROGRAM} -C ${KERNEL_DIR} M=${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
        OUTPUT mini-fs.ko
        COMMAND ${build_module} modules
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${MINI_FS_MODULE_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/Kbuild
        VERBATIM
)

add_custom_target(module DEPENDS mini-fs.ko)
add_custom_target(module-clean COMMAND ${build_module} clean)
