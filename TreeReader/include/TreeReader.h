#ifndef TREE_READER_H_
#define TREE_READER_H_

#include "NameTable.h"
#include "SyntaxTree.h"

enum class TranslationError {
    NO_ERRORS         = 0,
    CONTEXT_ERROR     = 1 << 0,
    NAME_TABLE_ERROR  = 1 << 1,
    INPUT_FILE_ERROR  = 1 << 2,
    DUMP_ERROR        = 1 << 3,
    OUTPUT_FILE_ERROR = 1 << 4,
    TREE_ERROR        = 1 << 5,
};

TranslationError ReadSyntaxTree (Tree::Tree <AstNode> *tree, char *fileContent);
TranslationError ReadNameTables (Buffer <NameTableRecord> *globalTable, Buffer <LocalNameTable> *localTables, char *fileContent);

#endif
