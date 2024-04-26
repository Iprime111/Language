project (backend)

add_executable (${PROJECT_NAME} main.cpp)

target_compile_definitions (${PROJECT_NAME} PRIVATE _SHOW_STACK_TRACE)

target_compile_options (${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
target_compile_options (${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:${RELEASE_FLAGS}>)

target_link_options (${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
target_link_options (${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:${RELEASE_FLAGS}>)

target_link_libraries (${PROJECT_NAME} PRIVATE Tree)
target_link_libraries (${PROJECT_NAME} PRIVATE ConsoleParser)
target_link_libraries (${PROJECT_NAME} PRIVATE Buffer)

target_link_libraries (${PROJECT_NAME} PRIVATE TreeReader)
target_link_libraries (${PROJECT_NAME} PRIVATE SyntaxTree)

add_subdirectory (src)
target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
