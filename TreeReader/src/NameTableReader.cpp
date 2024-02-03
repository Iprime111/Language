
#include "Buffer.h"
#include "Logger.h"
#include "NameTable.h"
#include "TreeReader.h"

static TranslationError ReadGlobalTable (Buffer <NameTableRecord> *globalTable, char **fileContent);
static TranslationError ReadLocalTable (Buffer <LocalNameTable> *localTables, size_t tableIndex, char **fileContent);

TranslationError ReadNameTables (Buffer <NameTableRecord> *globalTable, Buffer <LocalNameTable> *localTables, char *fileContent) {
    PushLog (3);

    ReadGlobalTable (globalTable, &fileContent);

    size_t tablesCount       = 0;
    int    countNumberLength = 0;

    sscanf (fileContent, "%lu%n", &tablesCount, &countNumberLength);

    fileContent += countNumberLength;

    for (size_t tableIndex = 0; tableIndex  < tablesCount; tableIndex++) {

        AddLocalNameTable (0, localTables);

        ReadLocalTable (localTables, tableIndex, &fileContent);
    }

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError ReadGlobalTable (Buffer <NameTableRecord> *globalTable, char **fileContent) {
    PushLog (3);

    size_t tableSize        = 0;
    int    sizeNumberLength = 0;

    sscanf (*fileContent, "%lu%n", &tableSize, &sizeNumberLength);

    (*fileContent) += sizeNumberLength;

    InitNameTable (globalTable, false);

    for (size_t globalTableRecord = 0; globalTableRecord < tableSize; globalTableRecord++) {
        
        char *newIdentifier  = NULL;
        sscanf (*fileContent, "%ms[^\n]", &newIdentifier);
        
        (*fileContent) += strlen (newIdentifier) + 1;
        
        AddIdentifier (globalTable, newIdentifier);
    }

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError ReadLocalTable (Buffer <LocalNameTable> *localTables, size_t tableIndex, char **fileContent) {
    PushLog (3);

    size_t tableSize        = 0;
    int    nameTableId      = 0;
    int    sizeNumberLength = 0;

    sscanf (*fileContent, "%lu %d%n", &tableSize, &nameTableId, &sizeNumberLength);

    localTables->data [tableIndex].nameTableId = nameTableId;

    (*fileContent) += sizeNumberLength;

    fprintf (stderr, "Name table size: %lu, ID: %d\n", tableSize, nameTableId);

    for (size_t tableRecord = 0; tableRecord < tableSize; tableRecord++) {

        size_t globalTableIndex = 0;
        size_t nameType         = 0;
        int    infoLength       = 0;

        sscanf (*fileContent, "%lu %lu%n", &globalTableIndex, &nameType, &infoLength);

        (*fileContent) += infoLength;

        AddLocalIdentifier (tableIndex, localTables, 
                            LocalNameTableRecord {.nameType = (LocalNameType) nameType, .globalNameId = globalTableIndex}, 0);
    }

    RETURN TranslationError::NO_ERRORS;

}
