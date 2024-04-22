#include <cassert>

#include "ParserBasics.h"
#include "FrontendCore.h"
#include "NameTable.h"
#include "SyntaxTree.h"

Tree::Node <AstNode> *GetStringToken (CompilationContext *context, NameType type, CompilationError error) {
    assert (context);

    SyntaxAssert (currentToken->nodeData.type == NodeType::STRING && context->nameTable.data [currentNameTableIndex].type == type, error);

    Tree::Node <AstNode> *returnValue = currentToken;
    context->tokenIndex++;

    return returnValue;
}

Tree::Node <AstNode> *GetKeyword (CompilationContext *context, Keyword keyword, CompilationError error) {
    assert (context);

    SyntaxAssert (currentToken->nodeData.type == NodeType::STRING && context->nameTable.data [currentNameTableIndex].keyword == keyword, error);

    Tree::Node <AstNode> *returnValue = currentToken;
    context->tokenIndex++;

    return returnValue;
}

Tree::Node <AstNode> *GetConstant (CompilationContext *context) {
    assert (context);

    SyntaxAssert (currentToken->nodeData.type == NodeType::CONSTANT, CompilationError::CONSTANT_EXPECTED);

    Tree::Node <AstNode> *returnValue = currentToken;
    context->tokenIndex++;

    return returnValue;
}

bool GetTokenAndDestroy (CompilationContext *context, Keyword keyword, CompilationError error) {
    assert (context);

    if (!GetKeyword (context, keyword, error)) {
        return false;
    }

    context->tokenIndex--;
    DestroyCurrentNode ();
    context->tokenIndex++;

    return true;
}

bool IsIdentifierDeclared (CompilationContext *context, int localNameTableId, size_t identifierIndex, LocalNameType identifierType) {
    assert (context);

    if (IsLocalIdentifierDeclared (context, localNameTableId, identifierIndex, identifierType)) {
        return true;
    }

    int nameIndex = GetIndexInLocalTable (0, &context->localTables, identifierIndex, identifierType);

    if (nameIndex >= 0) {
        return true;
    }
    
    return false;
}

bool IsLocalIdentifierDeclared (CompilationContext *context, int localNameTableId, size_t identifierIndex, LocalNameType identifierType) {
    assert (context);

    int localNameIndex = GetIndexInLocalTable (localNameTableId, &context->localTables, identifierIndex, identifierType);

    if (localNameIndex >= 0) {
        return true;
    }

    return false;
}
