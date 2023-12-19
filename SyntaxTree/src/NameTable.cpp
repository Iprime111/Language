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

BufferErrorCode AddIdentifier (Buffer <NameTableRecord> *nameTable, char *identifier) {
    PushLog (4);

    custom_assert (nameTable, pointer_is_null, BufferErrorCode::NO_BUFFER);

    NameTableRecord newRecord = {.name = identifier, .type = NameType::IDENTIFIER, .keyword = Keyword::NOT_KEYWORD};

    RETURN WriteDataToBuffer (nameTable, &newRecord, 1);
}
