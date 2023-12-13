project (frontend)

add_executable (${PROJECT_NAME} main.cpp)

target_link_libraries (${PROJECT_NAME} PRIVATE CustomAssert)
target_link_libraries (${PROJECT_NAME} PRIVATE Tree)
target_link_libraries (${PROJECT_NAME} PRIVATE Buffer)

target_link_libraries (${PROJECT_NAME} PRIVATE SyntaxTree)

add_subdirectory (src)
target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
