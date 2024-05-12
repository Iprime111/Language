add_subdirectory (AST)
add_subdirectory (Codegen)
add_subdirectory (TreeReader)

target_sources (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Dump.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/Opcodes.cpp)
