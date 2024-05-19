project (IRLib)

if (NOT TARGET IRLib)

    add_library (${PROJECT_NAME})

    add_subdirectory (src)
    target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

endif ()
