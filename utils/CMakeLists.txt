set(LIBRARY_NAME utils)

option(BUILD_SHARED_LIBS True)

add_library(${LIBRARY_NAME} headers/string_utils.h source/string_utils.c)

target_include_directories(
        ${LIBRARY_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> #export.h
        $<INSTALL_INTERFACE:headers> #${CMAKE_INSTALL_PREFIX}/headers
)

include(GenerateExportHeader)
generate_export_header(${LIBRARY_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/export.h DESTINATION headers)

add_subdirectory(headers)
add_subdirectory(source)
