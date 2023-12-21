#include <stddef.h>

#include "Parser.h"
#include "Buffer.h"
#include "ExpressionParser.h"
#include "FrontendCore.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "ParserBasics.h"
#include "TreeDefinitions.h"

static Tree::Node <AstNode> *GetGrammar               (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetTranslationUnit       (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetExternalDeclaration   (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetFunctionDefinition    (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetDeclaration           (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetOperator              (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetInitializerDeclarator (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetAssignmentExpression  (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetConditionOperator     (CompilationContext *context, Keyword operatorKeyword, CompilationError error, int localTableIndex);
static Tree::Node <AstNode> *GetOperatorList          (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetArgumentList          (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetParameterList         (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetReturnOperator        (CompilationContext *context, int localTableIndex);
static Tree::Node <AstNode> *GetOutOperator           (CompilationContext *context, int localTableIndex);

//---------------------------------------------------------------------------------------------------------------------------------------------------

CompilationError ParseCode (CompilationContext *context) {
    PushLog (1);

    context->tokenIndex = 0;
    context->abstractSyntaxTree.root = GetGrammar (context, 0);

    RETURN context->error;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static Tree::Node <AstNode> *GetGrammar (CompilationContext *context, int localTableIndex) {
    PushLog (1);

    NotNull (GetDestroyableToken(context, Keyword::INITIAL_OPERATOR, CompilationError::INITIAL_OPERATOR_EXPECTED));
    
    Tree::Node <AstNode> *rootNode = GetTranslationUnit (context, localTableIndex);
    DestroyCurrentNode ();

    RETURN rootNode;
}

static Tree::Node <AstNode> *GetTranslationUnit (CompilationContext *context, int localTableIndex) {
    PushLog (2);

    Tree::Node <AstNode> *externalDeclaration = GetExternalDeclaration (context, localTableIndex);

    NotNull (externalDeclaration);

    SyntaxAssert (currentToken->nodeData.type == NodeType::NAME && 
                    context->nameTable.data [currentNameTableIndex].keyword == Keyword::OPERATOR_SEPARATOR, 
                    CompilationError::OPERATOR_SEPARATOR_EXPECTED);

    currentToken->left          = externalDeclaration;
    externalDeclaration->parent = currentToken;

    Tree::Node <AstNode> *root = currentToken;

    context->tokenIndex++;

    if (currentToken->nodeData.type != NodeType::TERMINATOR) {
        root->right = GetTranslationUnit (context, localTableIndex);
        
        if (root->right)
            root->right->parent = root;
    }

    RETURN root;
}

static Tree::Node <AstNode> *GetExternalDeclaration (CompilationContext *context, int localTableIndex) {
    PushLog (2);

    Tree::Node <AstNode> *root = GetFunctionDefinition (context, localTableIndex);

    if (root) {
        RETURN root;
    }

    CheckForError (root, CompilationError::FUNCTION_EXPECTED);

    root = GetDeclaration (context, localTableIndex);

    RETURN root;
}

static Tree::Node <AstNode> *GetFunctionDefinition (CompilationContext *context, int localTableIndex) {
    PushLog (2);

    NotNull (GetDestroyableToken (context, Keyword::FUNCTION_DEFINITION, CompilationError::FUNCTION_EXPECTED));

    Tree::Node <AstNode> *type = GetNameWithType (context, NameType::TYPE_NAME, CompilationError::TYPE_NAME_EXPECTED);
    NotNull (type);

    Tree::Node <AstNode> *identifier = GetFunctionIdentifier (context, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);

    size_t identifierIndex = identifier->nodeData.content.nameTableIndex;

    context->tokenIndex--;
    DestroyCurrentNode ();
    context->tokenIndex++;

    NotNull (GetDestroyableToken (context, Keyword::LBRACKET,   CompilationError::BRACKET_EXPECTED));

    int newLocalTableIndex = AddLocalNameTable ((int) identifierIndex, &context->localTables);
    AddLocalIdentifier (0, &context->localTables, LocalNameTableRecord {.nameType = LocalNameType::FUNCTION_IDENTIFIER, .globalNameId = identifierIndex}, 0);

    Tree::Node <AstNode> *parameters = GetParameterList (context, newLocalTableIndex);
    CheckForError (parameters, CompilationError::TYPE_NAME_EXPECTED);

    NotNull (GetDestroyableToken (context, Keyword::RBRACKET,   CompilationError::BRACKET_EXPECTED));
    NotNull (GetDestroyableToken (context, Keyword::BLOCK_OPEN, CompilationError::CODE_BLOCK_EXPECTED));

    Tree::Node <AstNode> *functionContent = GetOperatorList (context, newLocalTableIndex);

    NotNull (GetDestroyableToken (context, Keyword::BLOCK_CLOSE, CompilationError::CODE_BLOCK_EXPECTED));

    RETURN FunctionDefinition (type, FunctionArguments (parameters, functionContent), identifierIndex);
}

static Tree::Node <AstNode> *GetDeclaration (CompilationContext *context, int localTableIndex) {
    PushLog (2);
   
    Tree::Node <AstNode> *type = GetNameWithType (context, NameType::TYPE_NAME, CompilationError::TYPE_NAME_EXPECTED);
    NotNull (type);

    Tree::Node <AstNode> *initializerDeclarator = GetInitializerDeclarator (context, localTableIndex);
    NotNull (initializerDeclarator);

    size_t identifierIndex = 0;

    if (context->nameTable.data [initializerDeclarator->nodeData.content.nameTableIndex].type == NameType::IDENTIFIER) {
        identifierIndex = initializerDeclarator->nodeData.content.nameTableIndex;
    } else {
        identifierIndex = initializerDeclarator->right->nodeData.content.nameTableIndex;
    }

    AddLocalIdentifier (localTableIndex, &context->localTables, LocalNameTableRecord {.nameType = LocalNameType::VARIABLE_IDENTIFIER, .globalNameId = identifierIndex}, 1);

    RETURN VariableDeclaration (type, initializerDeclarator, identifierIndex);
}

static Tree::Node <AstNode> *GetInitializerDeclarator (CompilationContext *context, int localTableIndex) {
    PushLog (2);

    Tree::Node <AstNode> *initializer = GetAssignmentExpression (context, localTableIndex);

    if (initializer) {
        RETURN initializer;
    }

    CheckForError (initializer, CompilationError::ASSIGNMENT_EXPECTED);
    context->tokenIndex--;

    initializer = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);

    RETURN initializer;
}

static Tree::Node <AstNode> *GetAssignmentExpression (CompilationContext *context, int localTableIndex) {
    PushLog (2);

    Tree::Node <AstNode> *identifier = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);

    Tree::Node <AstNode> *assignmentOperation = GetKeyword (context, Keyword::ASSIGNMENT, CompilationError::ASSIGNMENT_EXPECTED);
    NotNull (assignmentOperation);

    Tree::Node <AstNode> *expression = GetExpression (context, localTableIndex);
    NotNull (expression);

    assignmentOperation->left = expression;
    expression->parent        = assignmentOperation;

    assignmentOperation->right = identifier;
    identifier->parent         = assignmentOperation;

    RETURN assignmentOperation;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static Tree::Node <AstNode> *GetOperator (CompilationContext *context, int localTableIndex) {
    PushLog (2);

    Tree::Node <AstNode> *expectedOperator = NULL;

    expectedOperator = GetConditionOperator (context, Keyword::IF, CompilationError::IF_EXPECTED, localTableIndex);
    CheckForError (expectedOperator, CompilationError::IF_EXPECTED);

    if (expectedOperator) {
        RETURN OperatorSeparator (expectedOperator, NULL);   
    }
    
    expectedOperator = GetConditionOperator (context, Keyword::WHILE, CompilationError::WHILE_EXPECTED, localTableIndex);
    CheckForError (expectedOperator, CompilationError::WHILE_EXPECTED);
    
    if (expectedOperator) {
        RETURN OperatorSeparator (expectedOperator, NULL);   
    }

    while (true) {
        expectedOperator = GetKeyword (context, Keyword::ABORT, CompilationError::ABORT_EXPECTED);
        TryGetOperator (ABORT_EXPECTED);

        expectedOperator = GetOutOperator (context, localTableIndex);
        TryGetOperator (OUT_EXPECTED);

        expectedOperator = GetKeyword (context, Keyword::BREAK_OPERATOR, CompilationError::BREAK_EXPECTED);
        TryGetOperator (BREAK_EXPECTED);

        expectedOperator = GetKeyword (context, Keyword::CONTINUE_OPERATOR, CompilationError::CONTINUE_EXPECTED);
        TryGetOperator (CONTINUE_EXPECTED);

        expectedOperator = GetReturnOperator (context, localTableIndex);
        TryGetOperator (RETURN_EXPECTED);

        expectedOperator = GetFunctionCall (context, localTableIndex);
        TryGetOperator (FUNCTION_CALL_EXPECTED);

        expectedOperator = GetAssignmentExpression (context, localTableIndex);
        TryGetOperator (IDENTIFIER_EXPECTED);
        
        expectedOperator = GetDeclaration (context, localTableIndex);
        TryGetOperator (TYPE_NAME_EXPECTED);

        NotNull (GetDestroyableToken (context, Keyword::BLOCK_OPEN,  CompilationError::CODE_BLOCK_EXPECTED));
        expectedOperator = GetOperatorList (context, localTableIndex);
        NotNull (GetDestroyableToken (context, Keyword::BLOCK_CLOSE, CompilationError::CODE_BLOCK_EXPECTED));

        if (expectedOperator)
            break;

        RETURN NULL;
    }

    Tree::Node <AstNode> *separator = GetKeyword (context, Keyword::OPERATOR_SEPARATOR, CompilationError::OPERATOR_SEPARATOR_EXPECTED);
    NotNull (separator);

    separator->left          = expectedOperator;
    expectedOperator->parent = separator;

    RETURN separator;
}

static Tree::Node <AstNode> *GetOperatorList (CompilationContext *context, int localTableIndex) {
    PushLog (2);
    
    Tree::Node <AstNode> *firstOperator  = GetOperator (context, localTableIndex);
    NotNull (firstOperator);

    Tree::Node <AstNode> *secondOperator = GetOperatorList (context, localTableIndex);

    firstOperator->right = secondOperator;

    if (!secondOperator) {
        context->errorList.currentIndex--;
    } else {
        secondOperator->parent = firstOperator;
    }

    RETURN firstOperator;
}

static Tree::Node <AstNode> *GetConditionOperator (CompilationContext *context, Keyword operatorKeyword, CompilationError error, int localTableIndex) {
    PushLog (2);

    Tree::Node <AstNode> *conditionOperator = GetKeyword (context, operatorKeyword, error);
    NotNull (conditionOperator);

    Tree::Node <AstNode> *conditionExpression = GetExpression (context, localTableIndex);
    NotNull (conditionExpression);

    NotNull (GetDestroyableToken (context, Keyword::CONDITION_SEPARATOR, CompilationError::CONDITION_SEPARATOR_EXPECTED));

    Tree::Node <AstNode> *operatorContent = GetOperator (context, localTableIndex);
    NotNull (operatorContent);

    conditionOperator->left     = conditionExpression;
    conditionExpression->parent = conditionOperator;

    conditionOperator->right = operatorContent;
    operatorContent->parent  = conditionOperator;

    RETURN conditionOperator;
}

static Tree::Node <AstNode> *GetReturnOperator (CompilationContext *context, int localTableIndex) {
    PushLog (2);

    Tree::Node <AstNode> *returnStatement = GetKeyword (context, Keyword::RETURN_OPERATOR, CompilationError::RETURN_EXPECTED);
    NotNull (returnStatement);

    Tree::Node <AstNode> *expression = GetExpression (context, localTableIndex);
    NotNull (expression);

    returnStatement->right = expression;
    expression->parent     = returnStatement;

    RETURN returnStatement;
}

static Tree::Node <AstNode> *GetOutOperator (CompilationContext *context, int localTableIndex) {
    PushLog (2);

    Tree::Node <AstNode> *outOperator = GetKeyword (context, Keyword::OUT, CompilationError::OUT_EXPECTED);
    NotNull (outOperator);

    Tree::Node <AstNode> *expression = GetExpression (context, localTableIndex);
    NotNull (expression);

    outOperator->right = expression;
    expression->parent = outOperator;

    RETURN outOperator;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

Tree::Node <AstNode> *GetFunctionCall (CompilationContext *context, int localTableIndex) {
    PushLog (2);

    NotNull (GetDestroyableToken (context, Keyword::FUNCTION_CALL, CompilationError::FUNCTION_CALL_EXPECTED));

    Tree::Node <AstNode> *identifier = GetFunctionIdentifier (context, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);

    NotNull (GetDestroyableToken (context, Keyword::LBRACKET, CompilationError::BRACKET_EXPECTED));

    Tree::Node <AstNode> *arguments = GetArgumentList (context, localTableIndex);
    CheckForError (arguments, CompilationError::CONSTANT_EXPECTED);

    NotNull (GetDestroyableToken (context, Keyword::RBRACKET, CompilationError::BRACKET_EXPECTED));

    RETURN FunctionCall (arguments, identifier);
}

static Tree::Node <AstNode> *GetArgumentList (CompilationContext *context, int localTableIndex) {
    PushLog (2);

    Tree::Node <AstNode> *argument = GetExpression (context, localTableIndex);
    NotNull (argument);

    Tree::Node <AstNode> *separator = GetKeyword (context, Keyword::ARGUMENT_SEPARATOR, CompilationError::ARGUMENT_SEPARATOR_EXPECTED);
    
    if (!separator) {
        context->errorList.currentIndex--;
        RETURN ArgumentSeparator (argument, NULL);
    }
    
    Tree::Node <AstNode> *nextArgument = GetArgumentList (context, localTableIndex);
    NotNull (nextArgument);

    separator->left  = argument;
    argument->parent = separator;

    separator->right     = nextArgument;
    nextArgument->parent = separator;
    
    RETURN separator;
}

static Tree::Node <AstNode> *GetParameterList (CompilationContext *context, int localTableIndex) {
     PushLog (2);

    Tree::Node <AstNode> *parameter = GetDeclaration (context, localTableIndex);
    NotNull (parameter);

    Tree::Node <AstNode> *separator = GetKeyword (context, Keyword::ARGUMENT_SEPARATOR, CompilationError::ARGUMENT_SEPARATOR_EXPECTED);
    
    if (!separator) {
        context->errorList.currentIndex--;
        RETURN ArgumentSeparator (parameter, NULL);
    }
    
    Tree::Node <AstNode> *nextParameter = GetParameterList (context, localTableIndex);
    NotNull (nextParameter);

    separator->left   = parameter;
    parameter->parent = separator;

    separator->right      = nextParameter;
    nextParameter->parent = separator;
    
    RETURN separator;
}

