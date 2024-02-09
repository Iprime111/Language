#include "ExpressionParser.h"
#include "FrontendCore.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "ParserBasics.h"

static Tree::Node <AstNode> *GetBinaryOperation       (CompilationContext *context, size_t priority, int localNameTableId, bool onlyArythm);
static Tree::Node <AstNode> *GetUnaryOperation        (CompilationContext *context, size_t priority, int localNameTableId, bool onlyArythm);
static Tree::Node <AstNode> *GetPrimaryExpression     (CompilationContext *context, size_t priority, int localNameTableId, bool onlyArythm);
static Tree::Node <AstNode> *GetComparison            (CompilationContext *context, size_t priority, int localNameTableId, bool onlyArythm);
static Tree::Node <AstNode> *GetOperationWithPriority (CompilationContext *context, size_t priority, int localNameTableId);
static bool                  IsDiffOperation          (CompilationContext *context, Tree::Node <AstNode> *operation);
static Tree::Node <AstNode> *GetDerivative            (CompilationContext *context, int localNameTableId);

typedef Tree::Node <AstNode> *(* getter_t) (CompilationContext *, size_t, int, bool);

const size_t MAX_PRIORITY = 5;

static const getter_t NextFunction    [] = {GetPrimaryExpression, GetUnaryOperation, GetBinaryOperation, GetBinaryOperation, GetComparison, GetBinaryOperation};
static const size_t   OperationsCount [] = {0, 6, 2, 2, 6, 2};
static const Keyword  Operations   [][6] = {{},
                                            {Keyword::SIN,   Keyword::COS,           Keyword::NOT,        Keyword::FLOOR,   Keyword::SQRT, Keyword::SUB},
                                            {Keyword::MUL,   Keyword::DIV},
                                            {Keyword::ADD,   Keyword::SUB},
                                            {Keyword::EQUAL, Keyword::GREATER_EQUAL, Keyword::LESS_EQUAL, Keyword::GREATER, Keyword::LESS, Keyword::NOT_EQUAL},
                                            {Keyword::AND,   Keyword::OR}};

static const Keyword DiffOperations [7]  = {Keyword::ADD, Keyword::SUB, Keyword::MUL, Keyword::DIV, Keyword::SIN, Keyword::COS, Keyword::SQRT};
static const size_t  DiffOperationsCount = 7;

Tree::Node <AstNode> *GetExpression (CompilationContext *context, int localNameTableId) {
    PushLog (2);

    RETURN NextFunction [MAX_PRIORITY] (context, MAX_PRIORITY, localNameTableId, false);
}

static Tree::Node <AstNode> *GetBinaryOperation (CompilationContext *context, size_t priority, int localNameTableId, bool onlyArythm) {
    PushLog (3);

    Tree::Node <AstNode> *firstValue = NextFunction [priority - 1] (context, priority - 1, localNameTableId, onlyArythm);
    NotNull (firstValue);
    
    while (true) {
        Tree::Node <AstNode> *operation = GetOperationWithPriority (context, priority, localNameTableId);

        if (!operation) {
            RETURN firstValue;
        }

        if (onlyArythm) {
            SyntaxAssert (IsDiffOperation (context, operation), CompilationError::OPERATION_EXPECTED);
        }

        Tree::Node <AstNode> *secondValue = NextFunction [priority - 1] (context, priority - 1, localNameTableId, onlyArythm);
        NotNull (secondValue);

        operation->left    = firstValue;
        firstValue->parent = operation;

        operation->right    = secondValue;
        secondValue->parent = operation;

        firstValue = operation;
    }
}



static Tree::Node <AstNode> *GetUnaryOperation (CompilationContext *context, size_t priority, int localNameTableId, bool onlyArythm) {
    PushLog (3);

    Tree::Node <AstNode> *operation = GetOperationWithPriority (context, priority, localNameTableId);

    Tree::Node <AstNode> *value = NextFunction [priority - 1] (context, priority - 1, localNameTableId, onlyArythm);
    NotNull (value);

    if (operation) {
        if (onlyArythm) {
            SyntaxAssert (IsDiffOperation (context, operation), CompilationError::OPERATION_EXPECTED);
        }

        operation->right = value;
        value->parent    = operation;

        if (context->nameTable.data [operation->nodeData.content.nameTableIndex].keyword == Keyword::SUB) {
            operation->left         = Const (0);
            operation->left->parent = operation;
        }

        value = operation;
    }

    RETURN value;
}

static Tree::Node <AstNode> *GetComparison (CompilationContext *context, size_t priority, int localNameTableId, bool onlyArythm) {
    PushLog (3);
    
    Tree::Node <AstNode> *firstValue = NextFunction [priority - 1] (context, priority - 1, localNameTableId, onlyArythm);
    NotNull (firstValue);
    
    Tree::Node <AstNode> *operation = GetOperationWithPriority (context, priority, localNameTableId);

    if (!operation) {
        RETURN firstValue;
    }

    if (onlyArythm) {
        SyntaxAssert (IsDiffOperation (context, operation), CompilationError::OPERATION_EXPECTED);
    }

    Tree::Node <AstNode> *secondValue = NextFunction [priority - 1] (context, priority - 1, localNameTableId, onlyArythm);
    NotNull (secondValue);

    operation->left    = firstValue;
    firstValue->parent = operation;

    operation->right    = secondValue;
    secondValue->parent = operation;

    RETURN operation;
}

static Tree::Node <AstNode> *GetDerivative (CompilationContext *context, int localNameTableId) {
    PushLog (2);

    Tree::Node <AstNode> *derivativeNode = GetKeyword (context, Keyword::DIFF, CompilationError::DERIVATIVE_EXPECTED);
    NotNull (derivativeNode);

    NotNull (GetTokenAndDestroy (context, Keyword::LBRACKET, CompilationError::BRACKET_EXPECTED));

    Tree::Node <AstNode> *identifier = GetStringToken (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);
    DeclarationAssert (identifier, LocalNameType::VARIABLE_IDENTIFIER, CompilationError::VARIABLE_NOT_DECLARED);

    NotNull (GetTokenAndDestroy (context, Keyword::ARGUMENT_SEPARATOR, CompilationError::ARGUMENT_SEPARATOR_EXPECTED));

    Tree::Node <AstNode> *expression = NextFunction [MAX_PRIORITY] (context, MAX_PRIORITY, localNameTableId, true);
    NotNull (expression);

    NotNull (GetTokenAndDestroy (context, Keyword::RBRACKET, CompilationError::BRACKET_EXPECTED));

    derivativeNode->left  = identifier;
    identifier->parent    = derivativeNode;

    derivativeNode->right = expression;
    expression->parent    = derivativeNode;

    RETURN derivativeNode;
}

static Tree::Node <AstNode> *GetPrimaryExpression (CompilationContext *context, size_t priority, int localNameTableId, bool onlyArythm) {
    PushLog (3);

    if (GetTokenAndDestroy (context, Keyword::LBRACKET, CompilationError::BRACKET_EXPECTED)) {

        Tree::Node <AstNode> *expression = NextFunction [MAX_PRIORITY] (context, MAX_PRIORITY, localNameTableId, onlyArythm);

        NotNull (GetTokenAndDestroy (context, Keyword::RBRACKET, CompilationError::BRACKET_EXPECTED));

        RETURN expression;
    } else {
        context->errorBuffer.currentIndex--;
    }

    Tree::Node <AstNode> *functionCall = GetFunctionCall (context, localNameTableId);
    CheckForError (functionCall, CompilationError::FUNCTION_CALL_EXPECTED);

    if (functionCall) {
        RETURN functionCall;
    }

    Tree::Node <AstNode> *terminalSymbol = GetStringToken (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);

    if (terminalSymbol) {
        DeclarationAssert (terminalSymbol, LocalNameType::VARIABLE_IDENTIFIER, CompilationError::VARIABLE_NOT_DECLARED);

        RETURN terminalSymbol;
    }

    context->errorBuffer.currentIndex--;

    terminalSymbol = GetDerivative (context, localNameTableId);
    CheckForError (terminalSymbol, CompilationError::DERIVATIVE_EXPECTED);

    if (terminalSymbol) {
        RETURN terminalSymbol;
    }

    terminalSymbol = GetKeyword (context, Keyword::IN, CompilationError::IN_EXPECTED);

    if (terminalSymbol) {
        RETURN terminalSymbol;
    }

    context->errorBuffer.currentIndex--;

    terminalSymbol = GetConstant (context);

    RETURN terminalSymbol;
}

static Tree::Node <AstNode> *GetOperationWithPriority (CompilationContext *context, size_t priority, int localNameTableId) {
    PushLog (3);

    Tree::Node <AstNode> *operation = NULL;

    for (size_t operationIndex = 0; operationIndex < OperationsCount [priority]; operationIndex++) {
        operation = GetKeyword (context, Operations [priority][operationIndex], CompilationError::OPERATION_EXPECTED);

        if (!operation)
            context->errorBuffer.currentIndex--;
        else
            break;
    }

    RETURN operation;
}

static bool IsDiffOperation (CompilationContext *context, Tree::Node <AstNode> *operation) {
    PushLog (4);

    if (!operation || operation->nodeData.type != NodeType::STRING) {
        RETURN false;
    }

    for (size_t operationIndex = 0; operationIndex < DiffOperationsCount; operationIndex++) {
        if (context->nameTable.data [operation->nodeData.content.nameTableIndex].keyword == DiffOperations [operationIndex]) {
            RETURN true;
        }
    }

    RETURN false;
}
