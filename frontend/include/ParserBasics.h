#ifndef PARSER_BASICS_H_
#define PARSER_BASICS_H_
    #include "FrontendCore.h"
    #include "SyntaxTree.h"
    #include "TreeDefinitions.h"

    #define DeclarationAssert(IDENTIFIER, IDENTIFIER_TYPE, ERROR)                                               \
        SyntaxAssert (IsIdentifierDeclared (context, localNameTable,                                            \
                        IDENTIFIER->nodeData.content.nameTableIndex, IDENTIFIER_TYPE), ERROR)

    #define RedeclarationAssert(IDENTIFIER, IDENTIFIER_TYPE, ERROR)                                             \
        SyntaxAssert (!IsLocalIdentifierDeclared (context, localNameTable,                                      \
                        IDENTIFIER->nodeData.content.nameTableIndex, IDENTIFIER_TYPE), ERROR)

    #define TryGetOperator(ERROR)                                                                               \
        {                                                                                                       \
            CheckForError (expectedOperator, CompilationError::ERROR);                                          \
            if (expectedOperator) {                                                                             \
                break;                                                                                          \
            }                                                                                                   \
        }

    #define SyntaxAssert(EXPRESSION, ERROR)                                                                     \
        do {                                                                                                    \
            if (!(EXPRESSION)) {                                                                                \
                ErrorData newError = ErrorData {.error = ERROR,                                                 \
                    .line = context->tokens.data [context->tokenIndex]->nodeData.line,                          \
                    .file = context->tokens.data [context->tokenIndex]->nodeData.file};                         \
                WriteDataToBuffer (&context->errorList, &newError, 1);                                          \
                RETURN NULL;                                                                                    \
            }                                                                                                   \
        } while (0)
    
    #define currentToken context->tokens.data [context->tokenIndex]
    #define currentNameTableIndex currentToken->nodeData.content.nameTableIndex
    
    #define DestroyCurrentNode()                                                                                \
        do {                                                                                                    \
            Tree::DestroySingleNode (currentToken);                                                             \
            currentToken = NULL;                                                                                \
        } while (0)
    
    #define NotNull(EXPRESSION)                                                                                 \
        do {                                                                                                    \
            if (!(EXPRESSION)) {                                                                                \
                RETURN NULL;                                                                                    \
            }                                                                                                   \
        } while (0)
    
    #define CheckForError(NODE, ERROR)                                                                          \
        do {                                                                                                    \
            if (!(NODE)) {                                                                                      \
                if (context->errorList.data [context->errorList.currentIndex - 1].error != ERROR) {             \
                    RETURN NULL;                                                                                \
                }                                                                                               \
                context->errorList.currentIndex--;                                                              \
            }                                                                                                   \
        } while (0)

    Tree::Node <AstNode> *GetNameWithType (CompilationContext *context, NameType type,   CompilationError error);
    Tree::Node <AstNode> *GetKeyword      (CompilationContext *context, Keyword keyword, CompilationError error);
    Tree::Node <AstNode> *GetConstant     (CompilationContext *context);
    Tree::Node <AstNode> *GetFunctionCall (CompilationContext *context, int localNameTable);

    bool GetDestroyableToken       (CompilationContext *context, Keyword keyword, CompilationError error);
    bool IsIdentifierDeclared      (CompilationContext *context, int localNameTable, size_t identifierIndex, LocalNameType identifierType);
    bool IsLocalIdentifierDeclared (CompilationContext *context, int localNameTable, size_t identifierIndex, LocalNameType identifierType);
#endif 
