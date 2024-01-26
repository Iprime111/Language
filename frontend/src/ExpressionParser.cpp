#include "ExpressionParser.h"
#include "FrontendCore.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "ParserBasics.h"

static Tree::Node <AstNode> *GetBinaryOperation       (CompilationContext *context, size_t priority, int localNameTable);
static Tree::Node <AstNode> *GetUnaryOperation        (CompilationContext *context, size_t priority, int localNameTable);
static Tree::Node <AstNode> *GetPrimaryExpression     (CompilationContext *context, size_t priority, int localNameTable);
static Tree::Node <AstNode> *GetComparison            (CompilationContext *context, size_t priority, int localNameTable);
static Tree::Node <AstNode> *GetOperationWithPriority (CompilationContext *context, size_t priority, int localNameTable);

typedef Tree::Node <AstNode> *(* getter_t) (CompilationContext *, size_t, int);

const size_t MAX_PRIORITY = 5;

static const getter_t NextFunction    [] = {GetPrimaryExpression, GetUnaryOperation, GetBinaryOperation, GetBinaryOperation, GetComparison, GetBinaryOperation};
static const size_t   OperationsCount [] = {0, 3, 2, 2, 6, 2};
static const Keyword  Operations   [][6] = {{},
                                            {Keyword::SIN,   Keyword::COS,           Keyword::NOT},
                                            {Keyword::MUL,   Keyword::DIV},
                                            {Keyword::ADD,   Keyword::SUB},
                                            {Keyword::EQUAL, Keyword::GREATER_EQUAL, Keyword::LESS_EQUAL, Keyword::GREATER, Keyword::LESS, Keyword::NOT_EQUAL},
                                            {Keyword::AND,   Keyword::OR}};

Tree::Node <AstNode> *GetExpression (CompilationContext *context, int localNameTable) {
    PushLog (2);

    RETURN NextFunction [MAX_PRIORITY] (context, MAX_PRIORITY, localNameTable);
}

static Tree::Node <AstNode> *GetBinaryOperation (CompilationContext *context, size_t priority, int localNameTable) {
    PushLog (3);

    Tree::Node <AstNode> *firstValue = NextFunction [priority - 1] (context, priority - 1, localNameTable);
    NotNull (firstValue);
    
    while (true) {
        Tree::Node <AstNode> *operation = GetOperationWithPriority (context, priority, localNameTable);

        if (!operation) {
            RETURN firstValue;
        }

        Tree::Node <AstNode> *secondValue = NextFunction [priority - 1] (context, priority - 1, localNameTable);

        operation->left    = firstValue;
        firstValue->parent = operation;

        operation->right    = secondValue;
        secondValue->parent = operation;

        firstValue = operation;
    }
}



static Tree::Node <AstNode> *GetUnaryOperation (CompilationContext *context, size_t priority, int localNameTable) {
    PushLog (3);

    Tree::Node <AstNode> *operation = GetOperationWithPriority (context, priority, localNameTable);

    Tree::Node <AstNode> *value = NextFunction [priority - 1] (context, priority - 1, localNameTable);
    NotNull (value);

    if (operation) {
        operation->right = value;
        value->parent    = operation;

        value = operation;
    }

    RETURN value;
}

static Tree::Node <AstNode> *GetComparison (CompilationContext *context, size_t priority, int localNameTable) {
    PushLog (3);
    
    Tree::Node <AstNode> *firstValue = NextFunction [priority - 1] (context, priority - 1, localNameTable);
    NotNull (firstValue);
    
    Tree::Node <AstNode> *operation = GetOperationWithPriority (context, priority, localNameTable);

    if (!operation) {
        RETURN firstValue;
    }

    Tree::Node <AstNode> *secondValue = NextFunction [priority - 1] (context, priority - 1, localNameTable);

    operation->left    = firstValue;
    firstValue->parent = operation;

    operation->right    = secondValue;
    secondValue->parent = operation;

    RETURN operation;
}

static Tree::Node <AstNode> *GetPrimaryExpression (CompilationContext *context, size_t priority, int localNameTable) {
    PushLog (3);

    if (GetDestroyableToken (context, Keyword::LBRACKET, CompilationError::BRACKET_EXPECTED)) {

        Tree::Node <AstNode> *expression = NextFunction [MAX_PRIORITY] (context, MAX_PRIORITY, localNameTable);

        NotNull (GetDestroyableToken (context, Keyword::RBRACKET, CompilationError::BRACKET_EXPECTED));

        RETURN expression;
    } else {
        context->errorList.currentIndex--;
    }

    Tree::Node <AstNode> *functionCall = GetFunctionCall (context, localNameTable);
    CheckForError (functionCall, CompilationError::FUNCTION_CALL_EXPECTED);

    if (functionCall) {
        RETURN functionCall;
    }

    Tree::Node <AstNode> *terminalSymbol = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);

    if (terminalSymbol) {
        DeclarationAssert (terminalSymbol, LocalNameType::VARIABLE_IDENTIFIER, CompilationError::VARIABLE_NOT_DECLARED);

        RETURN terminalSymbol;
    }

    context->errorList.currentIndex--;

    terminalSymbol = GetKeyword (context, Keyword::IN, CompilationError::IN_EXPECTED);

    if (terminalSymbol) {
        RETURN terminalSymbol;
    }

    context->errorList.currentIndex--;

    terminalSymbol = GetConstant (context);

    RETURN terminalSymbol;
}

static Tree::Node <AstNode> *GetOperationWithPriority (CompilationContext *context, size_t priority, int localNameTable) {
    PushLog (3);

    Tree::Node <AstNode> *operation = NULL;

    for (size_t operationIndex = 0; operationIndex < OperationsCount [priority]; operationIndex++) {
        operation = GetKeyword (context, Operations [priority][operationIndex], CompilationError::OPERATION_EXPECTED);

        if (!operation)
            context->errorList.currentIndex--;
        else
            break;
    }

    RETURN operation;
}
