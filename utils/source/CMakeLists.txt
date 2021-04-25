set(
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers.c
        ${CMAKE_CURRENT_SOURCE_DIR}/io_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/net_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/string_utils.c
)
set(SUBDIRECTORIES "")
set(RELATIVE_PATH source)

#Add all subdirectories
foreach(VAR ${SUBDIRECTORIES})
    add_subdirectory(${VAR})
endforeach()

if(FILES)
    #Add source files to target
    target_sources(${LIBRARY_NAME} PRIVATE ${FILES})

    #Install our source files
    install(FILES ${FILES} DESTINATION ${RELATIVE_PATH})
endif()
