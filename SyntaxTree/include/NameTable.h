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
    const char *name    = nullptr;
    NameType    type    = NameType::IDENTIFIER;
    Keyword     keyword = Keyword::NOT_KEYWORD;
};

enum class LocalNameType {
    VARIABLE_IDENTIFIER = 1 << 0,
    FUNCTION_IDENTIFIER = 1 << 1,
};

struct LocalNameTableRecord {
    LocalNameType nameType     = LocalNameType::VARIABLE_IDENTIFIER;
    size_t        globalNameId = 0;
};

struct LocalNameTable {
    int    nameTableId = 0;
    size_t tableSize   = 0;

    Buffer <LocalNameTableRecord> items = {};
};

BufferErrorCode InitNameTable (Buffer <NameTableRecord> *nameTable, bool isGlobal);
BufferErrorCode AddIdentifier (Buffer <NameTableRecord> *nameTable, const char *identifier);

BufferErrorCode AddLocalIdentifier      (int nameTableIndex, Buffer <LocalNameTable> *localTables, LocalNameTableRecord newItem, size_t identifierSize);
int             AddLocalNameTable       (int nameTableId,    Buffer <LocalNameTable> *localTables);
int             GetIndexInLocalTable    (int nameTableIndex, Buffer <LocalNameTable> *localTables, size_t globalNameId, LocalNameType nameType);
int             GetLocalNameTableIndex  (int nameTableId,    Buffer <LocalNameTable> *localTables);

#endif
