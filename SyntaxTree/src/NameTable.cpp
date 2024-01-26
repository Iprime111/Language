#include "NameTable.h"
#include "Buffer.h"
#include "CustomAssert.h"
#include "Logger.h"

BufferErrorCode InitNameTable (Buffer <NameTableRecord> *nameTable, bool isGlobal) {
    PushLog (4);

    custom_assert (nameTable, pointer_is_null, BufferErrorCode::NO_BUFFER);

    if (InitBuffer (nameTable) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN BufferErrorCode::NO_BUFFER;
    }

    if (isGlobal) {
        #define KEYWORD(NAME, NUMBER, KEYWORD, TYPE, ...)                                                                   \
            {                                                                                                               \
                NameTableRecord newRecord = {.name = KEYWORD, .type = TYPE, .keyword = static_cast <Keyword> (NUMBER)};     \
                if (WriteDataToBuffer (nameTable, &newRecord, 1) != BufferErrorCode::NO_BUFFER_ERRORS) {                    \
                    RETURN BufferErrorCode::BUFFER_ENDED;                                                                   \
                }                                                                                                           \
            }

        #include "Keywords.def"

        #undef KEYWORD
    }

    RETURN BufferErrorCode::NO_BUFFER_ERRORS;
}

BufferErrorCode AddIdentifier (Buffer <NameTableRecord> *nameTable, const char *identifier) {
    PushLog (4);

    custom_assert (nameTable, pointer_is_null, BufferErrorCode::NO_BUFFER);

    NameTableRecord newRecord = {.name = identifier, .type = NameType::IDENTIFIER, .keyword = Keyword::NOT_KEYWORD};

    RETURN WriteDataToBuffer (nameTable, &newRecord, 1);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

int AddLocalNameTable (int nameTableId, Buffer <LocalNameTable> *localTables) {
    PushLog (4);

    custom_assert (localTables, pointer_is_null, -1);

    LocalNameTable newTable = {.nameTableId = nameTableId, .tableSize = 0, .items = {}};

    if (InitBuffer (&newTable.items) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN -1;
    }

    WriteDataToBuffer (localTables, &newTable, 1);

    RETURN localTables->currentIndex - 1;
}

BufferErrorCode AddLocalIdentifier (int nameTableIndex, Buffer <LocalNameTable> *localTables, LocalNameTableRecord newItem, size_t identifierSize) {
    PushLog (4);

    custom_assert (localTables, pointer_is_null, BufferErrorCode::NO_BUFFER);

    if (localTables->currentIndex >= nameTableIndex && nameTableIndex < 0) {
        RETURN BufferErrorCode::NO_BUFFER;
    }

    localTables->data [nameTableIndex].tableSize += identifierSize;

    RETURN WriteDataToBuffer (&localTables->data [nameTableIndex].items, &newItem, 1);
}

int GetLocalNameTableIndex (int nameTableId, Buffer <LocalNameTable> *localTables) {
    PushLog (4);

    custom_assert (localTables, pointer_is_null, -1);

    for (size_t tableIndex = 0; tableIndex < localTables->currentIndex; tableIndex++) {
        if (localTables->data [tableIndex].nameTableId == nameTableId) {
            RETURN tableIndex;
        }
    }

    RETURN -1;
}

int GetIndexInLocalTable (int nameTableIndex, Buffer <LocalNameTable> *localTables, size_t globalNameId, LocalNameType nameType) {
    PushLog (4);

    custom_assert (localTables, pointer_is_null, -1);

    if (localTables->currentIndex >= nameTableIndex && nameTableIndex < 0) {
        RETURN -1;
    }

    for (size_t nameIndex = 0; nameIndex < localTables->data [nameTableIndex].items.currentIndex; nameIndex++) {
        if (localTables->data [nameTableIndex].items.data [nameIndex].globalNameId == globalNameId &&
            ((int) localTables->data [nameTableIndex].items.data [nameIndex].nameType) & ((int) nameType)) {

            RETURN nameIndex;
        }
    }

    RETURN -1;
}
