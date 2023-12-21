#include "ParserBasics.h"
#include "FrontendCore.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"

Tree::Node <AstNode> *GetNameWithType (CompilationContext *context, NameType type, CompilationError error) {
    PushLog (3);

    SyntaxAssert (currentToken->nodeData.type == NodeType::NAME && context->nameTable.data [currentNameTableIndex].type == type, error);

    Tree::Node <AstNode> *returnValue = currentToken;
    context->tokenIndex++;

    RETURN returnValue;
}

Tree::Node <AstNode> *GetFunctionIdentifier (CompilationContext *context, CompilationError error) {
    PushLog (3);

    Tree::Node <AstNode> *identifier = GetNameWithType (context, NameType::IDENTIFIER, error);
    NotNull (identifier);

    SyntaxAssert (GetIndexInLocalTable (0, &context->localTables, identifier->nodeData.content.nameTableIndex, LocalNameType::FUNCTION_IDENTIFIER) >= 0, 
                    CompilationError::NO_FUNCTION);

    RETURN identifier;
}

Tree::Node <AstNode> *GetVariableIdentifier (CompilationContext *context, CompilationError error, int localTableIndex) {
    PushLog (3);

    Tree::Node <AstNode> *identifier = GetNameWithType (context, NameType::IDENTIFIER, error);
    NotNull (identifier);

    SyntaxAssert (GetIndexInLocalTable (localTableIndex, &context->localTables, identifier->nodeData.content.nameTableIndex, LocalNameType::VARIABLE_IDENTIFIER) >= 0 ||
                  GetIndexInLocalTable (0, &context->localTables, identifier->nodeData.content.nameTableIndex, LocalNameType::VARIABLE_IDENTIFIER) >= 0, 
                    CompilationError::NO_VARIABLE);

    RETURN identifier;

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
