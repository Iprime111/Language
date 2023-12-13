project (SyntaxTree)

add_library (${PROJECT_NAME})

target_sources (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/SyntaxTree.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/NameTable.cpp)

target_link_libraries (${PROJECT_NAME} PRIVATE Buffer)
target_link_libraries (${PROJECT_NAME} PRIVATE CustomAssert)
target_link_libraries (${PROJECT_NAME} PRIVATE Tree)

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
