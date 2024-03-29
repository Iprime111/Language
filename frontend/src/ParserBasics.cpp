#include "ParserBasics.h"
#include "FrontendCore.h"
#include "NameTable.h"
#include "SyntaxTree.h"

Tree::Node <AstNode> *GetStringToken (CompilationContext *context, NameType type, CompilationError error) {
    PushLog (3);

    SyntaxAssert (currentToken->nodeData.type == NodeType::STRING && context->nameTable.data [currentNameTableIndex].type == type, error);

    Tree::Node <AstNode> *returnValue = currentToken;
    context->tokenIndex++;

    RETURN returnValue;
}

Tree::Node <AstNode> *GetKeyword (CompilationContext *context, Keyword keyword, CompilationError error) {
    PushLog (3);

    SyntaxAssert (currentToken->nodeData.type == NodeType::STRING && context->nameTable.data [currentNameTableIndex].keyword == keyword, error);

    Tree::Node <AstNode> *returnValue = currentToken;
    context->tokenIndex++;

    RETURN returnValue;
}

Tree::Node <AstNode> *GetConstant (CompilationContext *context) {
    PushLog (3);

    SyntaxAssert (currentToken->nodeData.type == NodeType::CONSTANT, CompilationError::CONSTANT_EXPECTED);

    Tree::Node <AstNode> *returnValue = currentToken;
    context->tokenIndex++;

    RETURN returnValue;
}

bool GetTokenAndDestroy (CompilationContext *context, Keyword keyword, CompilationError error) {
    PushLog (3);

    if (!GetKeyword (context, keyword, error)) {
        RETURN false;
    }

    context->tokenIndex--;
    DestroyCurrentNode ();
    context->tokenIndex++;

    RETURN true;
}

bool IsIdentifierDeclared (CompilationContext *context, int localNameTableId, size_t identifierIndex, LocalNameType identifierType) {
    PushLog (3);

    if (IsLocalIdentifierDeclared (context, localNameTableId, identifierIndex, identifierType)) {
        RETURN true;
    }

    int nameIndex = GetIndexInLocalTable (0, &context->localTables, identifierIndex, identifierType);

    if (nameIndex >= 0) {
        RETURN true;
    }
    
    RETURN false;
}

bool IsLocalIdentifierDeclared (CompilationContext *context, int localNameTableId, size_t identifierIndex, LocalNameType identifierType) {
    PushLog (3);

    int localNameIndex = GetIndexInLocalTable (localNameTableId, &context->localTables, identifierIndex, identifierType);

    if (localNameIndex >= 0) {
        RETURN true;
    }

    RETURN false;
}
