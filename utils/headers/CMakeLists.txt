set(
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/defines.h
        ${CMAKE_CURRENT_SOURCE_DIR}/io_utils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/net_utils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/commands.h
)
set(SUBDIRECTORIES "")
set(RELATIVE_PATH headers)

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
