project (backend)

add_executable (${PROJECT_NAME} main.cpp)

target_compile_definitions (${PROJECT_NAME} PRIVATE _SHOW_STACK_TRACE)

target_compile_options (${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
target_compile_options (${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:${RELEASE_FLAGS}>)

target_link_options (${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
target_link_options (${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:${RELEASE_FLAGS}>)

target_link_libraries (${PROJECT_NAME} PRIVATE Tree)
target_link_libraries (${PROJECT_NAME} PRIVATE ConsoleParser)

target_link_libraries (${PROJECT_NAME} PRIVATE IRLib)

add_subdirectory (src)
target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

make_directory (${CMAKE_BINARY_DIR}/bin/Resources)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/asm/stdlib.asm ${CMAKE_BINARY_DIR}/bin/Resources/stdlib.asm COPYONLY)

