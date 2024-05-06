#include <cassert>

#include "NameTable.h"
#include "Buffer.h"

BufferErrorCode InitNameTable (Buffer <NameTableRecord> *nameTable, bool isGlobal) {
    assert (nameTable);

    if (InitBuffer (nameTable) != BufferErrorCode::NO_BUFFER_ERRORS)
        return BufferErrorCode::NO_BUFFER;

    if (isGlobal) {
        #define KEYWORD(NAME, NUMBER, KEYWORD, TYPE, ...)                                                                   \
            {                                                                                                               \
                NameTableRecord newRecord = {.name = KEYWORD, .type = TYPE, .keyword = static_cast <Keyword> (NUMBER)};     \
                if (WriteDataToBuffer (nameTable, &newRecord, 1) != BufferErrorCode::NO_BUFFER_ERRORS) {                    \
                    return BufferErrorCode::BUFFER_ENDED;                                                                   \
                }                                                                                                           \
            }

        #include "Keywords.def"

        #undef KEYWORD
    }

    return BufferErrorCode::NO_BUFFER_ERRORS;
}

BufferErrorCode AddIdentifier (Buffer <NameTableRecord> *nameTable, char *identifier) {
    assert (nameTable);
    assert (identifier);

    NameTableRecord newRecord = {.name = identifier, .type = NameType::IDENTIFIER, .keyword = Keyword::NOT_KEYWORD};

    return WriteDataToBuffer (nameTable, &newRecord, 1);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

int AddLocalNameTable (int nameTableId, Buffer <LocalNameTable> *localTables) {
    assert (localTables);

    LocalNameTable newTable = {.nameTableId = nameTableId, .tableSize = 0, .items = {}};

    if (InitBuffer (&newTable.items) != BufferErrorCode::NO_BUFFER_ERRORS)
        return -1;

    WriteDataToBuffer (localTables, &newTable, 1);

    return localTables->currentIndex - 1;
}

BufferErrorCode AddLocalIdentifier (int nameTableIndex, Buffer <LocalNameTable> *localTables, LocalNameTableRecord newItem, size_t identifierSize) {
    assert (localTables);

    if (localTables->currentIndex >= nameTableIndex && nameTableIndex < 0)
        return BufferErrorCode::NO_BUFFER;

    localTables->data [nameTableIndex].tableSize += identifierSize;

    return WriteDataToBuffer (&localTables->data [nameTableIndex].items, &newItem, 1);
}

int GetLocalNameTableIndex (int nameTableId, Buffer <LocalNameTable> *localTables) {
    assert (localTables);

    for (size_t tableIndex = 0; tableIndex < localTables->currentIndex; tableIndex++) {
        if (localTables->data [tableIndex].nameTableId == nameTableId)
            return tableIndex;
    }

    return -1;
}

int GetIndexInLocalTable (int nameTableIndex, Buffer <LocalNameTable> *localTables, size_t globalNameId, LocalNameType nameType) {
    assert (localTables);


    if (localTables->currentIndex >= nameTableIndex && nameTableIndex < 0)
        return -1;

    for (size_t nameIndex = 0; nameIndex < localTables->data [nameTableIndex].items.currentIndex; nameIndex++) {
        if (localTables->data [nameTableIndex].items.data [nameIndex].globalNameId == globalNameId &&
            ((int) localTables->data [nameTableIndex].items.data [nameIndex].nameType) & ((int) nameType)) {

            return nameIndex;
        }
    }

    return -1;
}
