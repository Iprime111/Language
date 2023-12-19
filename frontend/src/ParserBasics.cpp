#include "ParserBasics.h"
#include "FrontendCore.h"
#include "SyntaxTree.h"

Tree::Node <AstNode> *GetNameWithType (CompilationContext *context, NameType type, CompilationError error) {
    PushLog (3);

    SyntaxAssert (currentToken->nodeData.type == NodeType::NAME && context->nameTable.data [currentNameTableIndex].type == type, error);

    Tree::Node <AstNode> *returnValue = currentToken;
    context->tokenIndex++;

    RETURN returnValue;
}

Tree::Node <AstNode> *GetKeyword (CompilationContext *context, Keyword keyword, CompilationError error) {
    PushLog (3);

    SyntaxAssert (currentToken->nodeData.type == NodeType::NAME && context->nameTable.data [currentNameTableIndex].keyword == keyword, error);

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

bool GetDestroyableToken (CompilationContext *context, Keyword keyword, CompilationError error) {
    PushLog (3);

    if (!GetKeyword (context, keyword, error)) {
        RETURN false;
    }

    context->tokenIndex--;
    DestroyCurrentNode ();
    context->tokenIndex++;

    RETURN true;
}
