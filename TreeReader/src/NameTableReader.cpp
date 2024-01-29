
#include "Buffer.h"
#include "Logger.h"
#include "NameTable.h"
#include "TreeReader.h"

static TranslationError ReadGlobalTable (Buffer <NameTableRecord> *globalTable, char **fileContent);
static TranslationError ReadLocalTable (Buffer <LocalNameTable> *localTables, size_t tableIndex, char **fileContent);

TranslationError ReadNameTables (Buffer <NameTableRecord> *globalTable, Buffer <LocalNameTable> *localTables, char **fileContent) {
    PushLog (3);

    ReadGlobalTable (globalTable, fileContent);

    size_t tablesCount = 0;
    sscanf (*fileContent, "%lu", &tablesCount);

    for (size_t tableIndex = 0; tableIndex  < tablesCount; tableIndex++) {
        ReadLocalTable (localTables, tableIndex, fileContent);
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
        int identifierLength = 0; 
        sscanf (*fileContent, "%ms[^\n]%n", &newIdentifier, &identifierLength);
        
        (*fileContent) += identifierLength;
        
        AddIdentifier (globalTable, newIdentifier);
    }

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError ReadLocalTable (Buffer <LocalNameTable> *localTables, size_t tableIndex, char **fileContent) {
    PushLog (3);

    size_t tableSize        = 0;
    int    sizeNumberLength = 0;

    sscanf (*fileContent, "%lu%n", &tableSize, &sizeNumberLength);

    (*fileContent) += sizeNumberLength;

    for (size_t tableRecord = 0; tableRecord < tableSize; tableRecord++) {

        size_t globalTableIndex = 0;
        size_t nameType         = 0;
        int    infoLength       = 0;

        sscanf (*fileContent, "%lu %lu%n", &globalTableIndex, &nameType, &infoLength);

        (*fileContent) += infoLength;

        AddLocalIdentifier (tableIndex, localTables, 
                            LocalNameTableRecord {.nameType = (LocalNameType) nameType, .globalNameId = globalTableIndex}, 1);
    }

    RETURN TranslationError::NO_ERRORS;

}
