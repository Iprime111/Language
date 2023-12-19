#ifndef NAME_TABLE_H_
#define NAME_TABLE_H_

#include <stddef.h>

#include "Buffer.h"

enum class NameType {
    IDENTIFIER = 1 << 0,
    OPERATOR   = 1 << 1,
    TYPE_NAME  = 1 << 2,
    SEPARATOR  = 1 << 3,
};

#define KEYWORD(NAME, NUMBER, KEYWORD, TYPE, ...) NAME = NUMBER,

enum class Keyword {
    NOT_KEYWORD = 0,

    #include "Keywords.def"
};

#undef KEYWORD

struct NameTableRecord {
    char *name          = NULL;
    NameType    type    = NameType::IDENTIFIER;
    Keyword     keyword = Keyword::NOT_KEYWORD;
};

BufferErrorCode InitNameTable (Buffer <NameTableRecord> *nameTable, bool isGlobal);
BufferErrorCode AddIdentifier (Buffer <NameTableRecord> *nameTable, char *identifier);

#endif
