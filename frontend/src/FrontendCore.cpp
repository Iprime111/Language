#include <stdio.h>

#include "FrontendCore.h"
#include "Buffer.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeDefinitions.h"

static const char *NameTypeToString (NameType type);

CompilationError InitCompilationContext (CompilationContext *context) {
    PushLog (3);

    if (InitNameTable (&context->nameTable, true) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN CompilationError::NAME_TABLE_ERROR;
    }

    if (InitBuffer (&context->tokens) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN CompilationError::TOKEN_BUFFER_ERROR;
    }

    if (InitBuffer (&context->errorList) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN CompilationError::NAME_TABLE_ERROR; 
    }

    context->error = CompilationError::NO_ERRORS;

    RETURN CompilationError::NO_ERRORS;
}

CompilationError DestroyCompilationContext (CompilationContext *context) {
    PushLog (3);
    
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

    DestroyBuffer (&context->nameTable);
    DestroyBuffer (&context->errorList);
    DestroyBuffer (&context->tokens);

    RETURN CompilationError::NO_ERRORS;
}


// TODO: move lexer dumps to a separate file

CompilationError DumpTokenTable (CompilationContext *context) {
    PushLog (3);

    for (size_t tokenIndex = 0; tokenIndex < context->tokens.currentIndex; tokenIndex++) {
        DumpToken (context, context->tokens.data [tokenIndex]);
    }

    RETURN CompilationError::NO_ERRORS;
}

CompilationError DumpToken (CompilationContext *context, Tree::Node <AstNode> *token) {
    PushLog (3);

    if (token->nodeData.type == NodeType::CONSTANT) {
        printf ("Constant: %lg\n", token->nodeData.content.number);
    } else if (token->nodeData.type == NodeType::NAME) {
        printf ("Name: (type: \"%-10s\") <%s>\n", NameTypeToString (context->nameTable.data [token->nodeData.content.nameTableIndex].type), context->nameTable.data [token->nodeData.content.nameTableIndex].name);
    } else {
        printf ("Service node\n");
    }

    RETURN CompilationError::NO_ERRORS;
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

    return NULL;
}
