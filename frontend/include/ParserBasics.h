#ifndef PARSER_BASICS_H_
#define PARSER_BASICS_H_
    #include "FrontendCore.h"
    #include "SyntaxTree.h"
    #include "TreeDefinitions.h"

    #define TryGetOperator(ERROR)                                       \
        do {                                                            \
            CheckForError (expectedOperator, CompilationError::ERROR);  \
            if (expectedOperator) {                                     \
                break;                                                  \
            }                                                           \
        } while (0)



    #define SyntaxAssert(EXPRESSION, ERROR)                                                 \
        do {                                                                                \
            if (!(EXPRESSION)) {                                                            \
                ErrorData newError = ErrorData {.error = ERROR,                             \
                    .line = context->tokens.data [context->tokenIndex]->nodeData.line,      \
                    .file = context->tokens.data [context->tokenIndex]->nodeData.file};     \
                WriteDataToBuffer (&context->errorList, &newError, 1);                      \
                RETURN NULL;                                                                \
            }                                                                               \
        } while (0)
    
    #define currentToken context->tokens.data [context->tokenIndex]
    #define currentNameTableIndex currentToken->nodeData.content.nameTableIndex
    
    #define DestroyCurrentNode()                    \
        do {                                        \
            Tree::DestroySingleNode (currentToken); \
            currentToken = NULL;                    \
        } while (0)
    
    #define NotNull(EXPRESSION)     \
        do {                        \
            if (!(EXPRESSION)) {    \
                RETURN NULL;        \
            }                       \
        } while (0)
    
    #define CheckForError(NODE, ERROR)                                                              \
        do {                                                                                        \
            if (!(NODE)) {                                                                          \
                if (context->errorList.data [context->errorList.currentIndex - 1].error != ERROR) { \
                    RETURN NULL;                                                                    \
                }                                                                                   \
                context->errorList.currentIndex--;                                                  \
            }                                                                                       \
        } while (0)

    Tree::Node <AstNode> *GetNameWithType (CompilationContext *context, NameType type,   CompilationError error);
    Tree::Node <AstNode> *GetKeyword      (CompilationContext *context, Keyword keyword, CompilationError error);
    Tree::Node <AstNode> *GetConstant     (CompilationContext *context);
    Tree::Node <AstNode> *GetFunctionCall (CompilationContext *context);

    bool GetDestroyableToken (CompilationContext *context, Keyword keyword, CompilationError error);
#endif 
