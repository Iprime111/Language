#include <cassert>

#include "TreeReader/TreeReader.h"
#include "AST/TranslationContext.h"

namespace Ast {
    TreeReader::TreeReader (TranslationContext *context) : context (context) {}

    void TreeReader::ReadTree (FILE *stream) {
        
    }

    void TreeReader::ReadNameTables (char *fileContent) {
        assert (fileContent);
    
        ReadGlobalTable (&fileContent);
    
        size_t tablesCount       = 0;
        int    countNumberLength = 0;
    
        sscanf (fileContent, "%lu%n", &tablesCount, &countNumberLength);
    
        fileContent += countNumberLength;
    
        for (size_t tableIndex = 0; tableIndex  < tablesCount; tableIndex++) {
    
            AddLocalNameTable (0, &context->localTables);
    
            ReadLocalTable (&fileContent, tableIndex);
        }
    }

    void TreeReader::ReadGlobalTable (char **fileContent) {
        assert (fileContent);
    
        size_t tableSize        = 0;
        int    sizeNumberLength = 0;
    
        sscanf (*fileContent, "%lu%n", &tableSize, &sizeNumberLength);
    
        (*fileContent) += sizeNumberLength;
    
        InitNameTable (&context->nameTable, false);
    
        for (size_t globalTableRecord = 0; globalTableRecord < tableSize; globalTableRecord++) {
            
            char *newIdentifier  = nullptr;
            int   identifierSize = 0;
            sscanf (*fileContent, "%ms%n[^\n]", &newIdentifier, &identifierSize);
            
            (*fileContent) += identifierSize;
            
            AddIdentifier (&context->nameTable, newIdentifier);
        }
    }
    
    void TreeReader::ReadLocalTable (char **fileContent, size_t tableIndex) {
        assert (fileContent);
    
        size_t tableSize        = 0;
        int    nameTableId      = 0;
        int    sizeNumberLength = 0;
    
        sscanf (*fileContent, "%lu %d%n", &tableSize, &nameTableId, &sizeNumberLength);
    
        context->localTables.data [tableIndex].nameTableId = nameTableId;
    
        (*fileContent) += sizeNumberLength;
    
        for (size_t tableRecord = 0; tableRecord < tableSize; tableRecord++) {
    
            size_t globalTableIndex = 0;
            size_t nameType         = 0;
            int    infoLength       = 0;
    
            sscanf (*fileContent, "%lu %lu%n", &globalTableIndex, &nameType, &infoLength);
    
            (*fileContent) += infoLength;
    
            AddLocalIdentifier ((int) tableIndex, &context->localTables, 
                                LocalNameTableRecord {.nameType = (LocalNameType) nameType, .globalNameId = globalTableIndex}, 0);
        }
    }
}
