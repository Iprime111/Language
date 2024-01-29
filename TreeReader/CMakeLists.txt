project (TreeReader)

if (NOT TARGET TreeReader)

add_library (${PROJECT_NAME})

target_sources (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/TreeReader.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/src/NameTableReader.cpp)

target_link_libraries (${PROJECT_NAME} PRIVATE Buffer)
target_link_libraries (${PROJECT_NAME} PRIVATE CustomAssert)
target_link_libraries (${PROJECT_NAME} PRIVATE Tree)
target_link_libraries (${PROJECT_NAME} PRIVATE SyntaxTree)

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

endif ()
