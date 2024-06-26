#include <cstring>
#include <stdio.h>
#include <cassert>

#include "FrontendCore.h"
#include "Buffer.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeDefinitions.h"

static const char *NameTypeToString (NameType type);

CompilationError InitCompilationContext (CompilationContext *context, char *fileContent) {
    assert (context);
    assert (fileContent);

    if (InitBuffer (&context->localTables) != BufferErrorCode::NO_BUFFER_ERRORS)
        return CompilationError::CONTEXT_ERROR;

    AddLocalNameTable (-1, &context->localTables);

    if (InitNameTable (&context->nameTable, true) != BufferErrorCode::NO_BUFFER_ERRORS)
        return CompilationError::CONTEXT_ERROR;

    if (InitBuffer (&context->tokens) != BufferErrorCode::NO_BUFFER_ERRORS)
        return CompilationError::TOKEN_BUFFER_ERROR;

    if (InitBuffer (&context->errorBuffer) != BufferErrorCode::NO_BUFFER_ERRORS)
        return CompilationError::CONTEXT_ERROR; 

     if (InitBuffer (&context->functionCalls) != BufferErrorCode::NO_BUFFER_ERRORS)
        return CompilationError::CONTEXT_ERROR; 

    context->error = CompilationError::NO_ERRORS;
    
    context->fileContent = fileContent;
    context->fileLength = strlen (fileContent);
    context->currentLine = 1;

    return CompilationError::NO_ERRORS;
}

CompilationError DestroyCompilationContext (CompilationContext *context) {
    assert (context);
    
    if (context->abstractSyntaxTree.root) {
        Tree::DestroySubtreeNode (&context->abstractSyntaxTree, context->abstractSyntaxTree.root);
    } else {
        for (size_t tokenIndex = 0; tokenIndex < context->tokens.currentIndex; tokenIndex++) {
            free (context->tokens.data [tokenIndex]);
        }
    }

    for (size_t nameIndex = 0; nameIndex < context->nameTable.currentIndex; nameIndex++) {
        if (context->nameTable.data [nameIndex].type == NameType::IDENTIFIER) {
            free (const_cast <char *> (context->nameTable.data [nameIndex].name));
        }
    }

    for (size_t localTableIndex = 0; localTableIndex < context->localTables.currentIndex; localTableIndex++)
        DestroyBuffer (&context->localTables.data [localTableIndex].items);

    DestroyBuffer (&context->localTables);
    DestroyBuffer (&context->nameTable);
    DestroyBuffer (&context->errorBuffer);
    DestroyBuffer (&context->tokens);
    DestroyBuffer (&context->functionCalls);

    free (context->fileContent);

    return CompilationError::NO_ERRORS;
}

// TODO: move lexer dumps to a separate file

CompilationError DumpTokenTable (CompilationContext *context) {
    assert (context);

    for (size_t tokenIndex = 0; tokenIndex < context->tokens.currentIndex; tokenIndex++)
        DumpToken (context, context->tokens.data [tokenIndex]);

    return CompilationError::NO_ERRORS;
}

CompilationError DumpToken (CompilationContext *context, Tree::Node <AstNode> *token) {
    assert (context);
    assert (token);

    if (token == nullptr) {
        printf ("null\n");

        return CompilationError::NO_ERRORS;
    }

    if (token->nodeData.type == NodeType::CONSTANT) {
        printf ("Constant: %lg\n", token->nodeData.content.number);

    } else if (token->nodeData.type == NodeType::STRING) {
        printf ("Name: (type: \"%-10s\") <%s>\n",
                NameTypeToString (context->nameTable.data [token->nodeData.content.nameTableIndex].type), 
                context->nameTable.data [token->nodeData.content.nameTableIndex].name);
    } else {
        printf ("Service node\n");
    }

    return CompilationError::NO_ERRORS;
}

static const char *NameTypeToString (NameType type) {
    switch (type) {

        case NameType::IDENTIFIER:
            return "identifier";
        
        case NameType::OPERATOR:
            return "operator";
        
        case NameType::TYPE_NAME:
            return "type name";
        
        case NameType::SEPARATOR:
            return "separator";
    }

    return nullptr;
}
