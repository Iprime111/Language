#include <cstdio>
#include <stddef.h>
#include <cassert>

#include "Parser.h"
#include "Buffer.h"
#include "ExpressionParser.h"
#include "FrontendCore.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "ParserBasics.h"
#include "TreeDefinitions.h"

static Tree::Node <AstNode> *GetGrammar               (CompilationContext *context);
static Tree::Node <AstNode> *GetTranslationUnit       (CompilationContext *context);
static Tree::Node <AstNode> *GetExternalDeclaration   (CompilationContext *context);
static Tree::Node <AstNode> *GetFunctionDefinition    (CompilationContext *context, int localNameTableId);
static Tree::Node <AstNode> *GetDeclaration           (CompilationContext *context, int localNameTableId);
static Tree::Node <AstNode> *GetOperator              (CompilationContext *context, int localNameTableId);
static Tree::Node <AstNode> *GetInitializerDeclarator (CompilationContext *context, int localNameTableId);
static Tree::Node <AstNode> *GetAssignmentExpression  (CompilationContext *context, int localNameTableId);
static Tree::Node <AstNode> *GetConditionOperator     (CompilationContext *context, Keyword operatorKeyword, CompilationError error, int localNameTableId);
static Tree::Node <AstNode> *GetOperatorList          (CompilationContext *context, int localNameTableId);
static Tree::Node <AstNode> *GetArgumentList          (CompilationContext *context, int localNameTableId);
static Tree::Node <AstNode> *GetParameterList         (CompilationContext *context, int localNameTableId);
static Tree::Node <AstNode> *GetReturnOperator        (CompilationContext *context, int localNameTableId);
static Tree::Node <AstNode> *GetOutOperator           (CompilationContext *context, int localNameTableId);

//---------------------------------------------------------------------------------------------------------------------------------------------------

CompilationError ParseCode (CompilationContext *context) {
    assert (context);

    context->tokenIndex = 0;
    context->abstractSyntaxTree.root = GetGrammar (context);

   return context->error;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static Tree::Node <AstNode> *GetGrammar (CompilationContext *context) {
    assert (context);

    NotNull (GetTokenAndDestroy (context, Keyword::INITIAL_OPERATOR, CompilationError::INITIAL_OPERATOR_EXPECTED));

    Tree::Node <AstNode> *entryPointIdentifier = GetStringToken (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (entryPointIdentifier);

    NotNull (GetTokenAndDestroy (context, Keyword::OPERATOR_SEPARATOR, CompilationError::OPERATOR_SEPARATOR_EXPECTED));

    context->entryPoint = entryPointIdentifier->nodeData.content.nameTableIndex;
    
    Tree::Node <AstNode> *rootNode = GetTranslationUnit (context);
    NotNull (rootNode);
    DestroyCurrentNode ();
    
    int localNameTableId = 0;
    for (size_t callIndex = 0; callIndex < context->functionCalls.currentIndex; callIndex++) {
        DeclarationAssert (context->functionCalls.data [callIndex], LocalNameType::FUNCTION_IDENTIFIER, CompilationError::FUNCTION_NOT_DECLARED);
    }

    Tree::DestroySingleNode (entryPointIdentifier);

    return rootNode;
}

static Tree::Node <AstNode> *GetTranslationUnit (CompilationContext *context) {
    assert (context);

    Tree::Node <AstNode> *externalDeclaration = GetExternalDeclaration (context);

    NotNull (externalDeclaration);

    //TODO: STRING
    SyntaxAssert (currentToken->nodeData.type == NodeType::STRING && 
                    context->nameTable.data [currentNameTableIndex].keyword == Keyword::OPERATOR_SEPARATOR, 
                    CompilationError::OPERATOR_SEPARATOR_EXPECTED);

    currentToken->left          = externalDeclaration;
    externalDeclaration->parent = currentToken;

    Tree::Node <AstNode> *root = currentToken;

    ++context->tokenIndex;

    if (currentToken->nodeData.type != NodeType::TERMINATOR) {
        root->right = GetTranslationUnit (context);
        
        if (root->right)
            root->right->parent = root;
    }

    return root;
}

static Tree::Node <AstNode> *GetExternalDeclaration (CompilationContext *context) {
    assert (context);

    Tree::Node <AstNode> *root = GetFunctionDefinition (context, 0);

    if (root) {
        return root;
    }

    CheckForError (root, CompilationError::FUNCTION_EXPECTED);

    root = GetDeclaration (context, 0);

    return root;
}

static Tree::Node <AstNode> *GetFunctionDefinition (CompilationContext *context, int localNameTableId) {
    assert (context);

    NotNull (GetTokenAndDestroy (context, Keyword::FUNCTION_DEFINITION, CompilationError::FUNCTION_EXPECTED));

    Tree::Node <AstNode> *type = GetStringToken (context, NameType::TYPE_NAME, CompilationError::TYPE_NAME_EXPECTED);
    NotNull (type);

    Tree::Node <AstNode> *identifier = GetStringToken (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);

    size_t identifierIndex = identifier->nodeData.content.nameTableIndex;

    RedeclarationAssert (identifier, 
                          (LocalNameType) ((int) LocalNameType::VARIABLE_IDENTIFIER | (int) LocalNameType::FUNCTION_IDENTIFIER), 
                          CompilationError::FUNCTION_REDEFINITION);

    --context->tokenIndex;
    DestroyCurrentNode ();
    ++context->tokenIndex;
     
    int newNameTableIndex = AddLocalNameTable ((int) identifierIndex, &context->localTables);
    AddLocalIdentifier (0, &context->localTables, 
                        LocalNameTableRecord {.nameType = LocalNameType::FUNCTION_IDENTIFIER, .globalNameId = identifierIndex}, 0);

    NotNull (GetTokenAndDestroy (context, Keyword::LBRACKET,   CompilationError::BRACKET_EXPECTED));

    Tree::Node <AstNode> *parameters = GetParameterList (context, newNameTableIndex);
    CheckForError (parameters, CompilationError::TYPE_NAME_EXPECTED);

    NotNull (GetTokenAndDestroy (context, Keyword::RBRACKET,   CompilationError::BRACKET_EXPECTED));
    NotNull (GetTokenAndDestroy (context, Keyword::BLOCK_OPEN, CompilationError::CODE_BLOCK_EXPECTED));

    Tree::Node <AstNode> *functionContent = GetOperatorList (context, newNameTableIndex);

    CheckForError (functionContent, CompilationError::OPERATOR_NOT_FOUND);

    NotNull (GetTokenAndDestroy (context, Keyword::BLOCK_CLOSE, CompilationError::CODE_BLOCK_EXPECTED));

    return FunctionDefinition (type, FunctionArguments (parameters, functionContent), identifierIndex);
}

static Tree::Node <AstNode> *GetDeclaration (CompilationContext *context, int localNameTableId) {
    assert (context);
   
    Tree::Node <AstNode> *type = GetStringToken (context, NameType::TYPE_NAME, CompilationError::TYPE_NAME_EXPECTED);
    NotNull (type);

    Tree::Node <AstNode> *identifier = GetStringToken (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);
    context->tokenIndex--;

    size_t identifierIndex = identifier->nodeData.content.nameTableIndex;

    RedeclarationAssert (identifier, 
                         (LocalNameType) ((int) LocalNameType::VARIABLE_IDENTIFIER | (int) LocalNameType::FUNCTION_IDENTIFIER), 
                         CompilationError::VARIABLE_REDECLARATION);

    AddLocalIdentifier (localNameTableId, &context->localTables, 
                        LocalNameTableRecord {.nameType = LocalNameType::VARIABLE_IDENTIFIER, .globalNameId = identifierIndex}, 1);
    

    Tree::Node <AstNode> *initializerDeclarator = GetInitializerDeclarator (context, localNameTableId);
    NotNull (initializerDeclarator);


    return VariableDeclaration (type, initializerDeclarator, identifierIndex);
}

static Tree::Node <AstNode> *GetInitializerDeclarator (CompilationContext *context, int localNameTableId) {
    assert (context);

    Tree::Node <AstNode> *initializer = GetAssignmentExpression (context, localNameTableId);

    if (initializer) {
        return initializer;
    }

    CheckForError (initializer, CompilationError::ASSIGNMENT_EXPECTED);
    context->tokenIndex--; 

    initializer = GetStringToken (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);

    return initializer;
}

static Tree::Node <AstNode> *GetAssignmentExpression (CompilationContext *context, int localNameTableId) {
    assert (context);

    Tree::Node <AstNode> *identifier = GetStringToken (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);
    DeclarationAssert (identifier, LocalNameType::VARIABLE_IDENTIFIER, CompilationError::VARIABLE_NOT_DECLARED);


    Tree::Node <AstNode> *assignmentOperation = GetKeyword (context, Keyword::ASSIGNMENT, CompilationError::ASSIGNMENT_EXPECTED);
    NotNull (assignmentOperation);

    Tree::Node <AstNode> *expression = GetExpression (context, localNameTableId);
    NotNull (expression);

    assignmentOperation->left = expression;
    expression->parent        = assignmentOperation;

    assignmentOperation->right = identifier;
    identifier->parent         = assignmentOperation;

    return assignmentOperation;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static Tree::Node <AstNode> *GetOperator (CompilationContext *context, int localNameTableId) {
    assert (context);

    Tree::Node <AstNode> *expectedOperator = NULL;

    expectedOperator = GetConditionOperator (context, Keyword::IF, CompilationError::IF_EXPECTED, localNameTableId);
    CheckForError (expectedOperator, CompilationError::IF_EXPECTED);

    if (expectedOperator) {
        return OperatorSeparator (expectedOperator, NULL);   
    }
    
    expectedOperator = GetConditionOperator (context, Keyword::WHILE, CompilationError::WHILE_EXPECTED, localNameTableId);
    CheckForError (expectedOperator, CompilationError::WHILE_EXPECTED);
    
    if (expectedOperator) {
        return OperatorSeparator (expectedOperator, NULL);   
    }

    #define GetExpectedOperator(FUNCTION, ERROR)                    \
        expectedOperator = FUNCTION (context, localNameTableId);    \
        TryGetOperator (ERROR)

    do {
        expectedOperator = GetKeyword (context, Keyword::ABORT, CompilationError::ABORT_EXPECTED);
        TryGetOperator (ABORT_EXPECTED);


        expectedOperator = GetKeyword (context, Keyword::BREAK_OPERATOR, CompilationError::BREAK_EXPECTED);
        TryGetOperator (BREAK_EXPECTED);

        expectedOperator = GetKeyword (context, Keyword::CONTINUE_OPERATOR, CompilationError::CONTINUE_EXPECTED);
        TryGetOperator (CONTINUE_EXPECTED);

        GetExpectedOperator (GetOutOperator,          OUT_EXPECTED);
        GetExpectedOperator (GetReturnOperator,       RETURN_EXPECTED);
        GetExpectedOperator (GetFunctionCall,         FUNCTION_CALL_EXPECTED);
        GetExpectedOperator (GetAssignmentExpression, IDENTIFIER_EXPECTED);
        GetExpectedOperator (GetDeclaration,          TYPE_NAME_EXPECTED);

        NotNull (GetTokenAndDestroy (context, Keyword::BLOCK_OPEN,  CompilationError::OPERATOR_NOT_FOUND));
        expectedOperator = GetOperatorList (context, localNameTableId);
        NotNull (expectedOperator);
        NotNull (GetTokenAndDestroy (context, Keyword::BLOCK_CLOSE, CompilationError::OPERATOR_NOT_FOUND));

    } while (0);

    Tree::Node <AstNode> *separator = GetKeyword (context, Keyword::OPERATOR_SEPARATOR, CompilationError::OPERATOR_SEPARATOR_EXPECTED);
    NotNull (separator);

    separator->left          = expectedOperator;
    expectedOperator->parent = separator;

    return separator;
}

static Tree::Node <AstNode> *GetOperatorList (CompilationContext *context, int localNameTableId) {
    assert (context);
    
    Tree::Node <AstNode> *firstOperator  = GetOperator     (context, localNameTableId);
    NotNull (firstOperator);

    Tree::Node <AstNode> *secondOperator = GetOperatorList (context, localNameTableId);

    firstOperator->right = secondOperator;

    CheckForError (secondOperator, CompilationError::OPERATOR_NOT_FOUND);

    if (secondOperator)
        secondOperator->parent = firstOperator;


    return firstOperator;
}

static Tree::Node <AstNode> *GetConditionOperator (CompilationContext *context, Keyword operatorKeyword, CompilationError error, int localNameTableId) {
    assert (context);

    Tree::Node <AstNode> *conditionOperator = GetKeyword (context, operatorKeyword, error);
    NotNull (conditionOperator);

    Tree::Node <AstNode> *conditionExpression = GetExpression (context, localNameTableId);
    NotNull (conditionExpression);

    NotNull (GetTokenAndDestroy (context, Keyword::CONDITION_SEPARATOR, CompilationError::CONDITION_SEPARATOR_EXPECTED));

    Tree::Node <AstNode> *operatorContent = GetOperator (context, localNameTableId);
    NotNull (operatorContent);

    conditionOperator->left     = conditionExpression;
    conditionExpression->parent = conditionOperator;

    conditionOperator->right = operatorContent;
    operatorContent->parent  = conditionOperator;

    return conditionOperator;
}

static Tree::Node <AstNode> *GetReturnOperator (CompilationContext *context, int localNameTableId) {
    assert (context);

    Tree::Node <AstNode> *returnStatement = GetKeyword (context, Keyword::RETURN_OPERATOR, CompilationError::RETURN_EXPECTED);
    NotNull (returnStatement);

    Tree::Node <AstNode> *expression = GetExpression (context, localNameTableId);
    NotNull (expression);

    returnStatement->right = expression;
    expression->parent     = returnStatement;

    return returnStatement;
}

static Tree::Node <AstNode> *GetOutOperator (CompilationContext *context, int localNameTableId) {
    assert (context);

    Tree::Node <AstNode> *outOperator = GetKeyword (context, Keyword::OUT, CompilationError::OUT_EXPECTED);
    NotNull (outOperator);

    Tree::Node <AstNode> *expression = GetExpression (context, localNameTableId);
    NotNull (expression);

    outOperator->right = expression;
    expression->parent = outOperator;

    return outOperator;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

Tree::Node <AstNode> *GetFunctionCall (CompilationContext *context, int localNameTableId) {
    assert (context);

    NotNull (GetTokenAndDestroy (context, Keyword::FUNCTION_CALL, CompilationError::FUNCTION_CALL_EXPECTED));

    Tree::Node <AstNode> *identifier = GetStringToken (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);
    WriteDataToBuffer (&context->functionCalls, &identifier, 1);

    NotNull (GetTokenAndDestroy (context, Keyword::LBRACKET, CompilationError::BRACKET_EXPECTED));

    Tree::Node <AstNode> *arguments = GetArgumentList (context, localNameTableId);
    CheckForError (arguments, CompilationError::CONSTANT_EXPECTED);

    NotNull (GetTokenAndDestroy (context, Keyword::RBRACKET, CompilationError::BRACKET_EXPECTED));

    return FunctionCall (arguments, identifier);
}

static Tree::Node <AstNode> *GetArgumentList (CompilationContext *context, int localNameTableId) {
    assert (context);

    Tree::Node <AstNode> *argument = GetExpression (context, localNameTableId);
    NotNull (argument);

    Tree::Node <AstNode> *separator = GetKeyword (context, Keyword::ARGUMENT_SEPARATOR, CompilationError::ARGUMENT_SEPARATOR_EXPECTED);
    
    if (!separator) {
        context->errorBuffer.currentIndex--;
        return ArgumentSeparator (argument, NULL);
    }
    
    Tree::Node <AstNode> *nextArgument = GetArgumentList (context, localNameTableId);
    NotNull (nextArgument);

    separator->left  = argument;
    argument->parent = separator;

    separator->right     = nextArgument;
    nextArgument->parent = separator;
    
    return separator;
}

static Tree::Node <AstNode> *GetParameterList (CompilationContext *context, int localNameTableId) {
    assert (context);

    Tree::Node <AstNode> *parameter = GetDeclaration (context, localNameTableId);
    NotNull (parameter);

    Tree::Node <AstNode> *separator = GetKeyword (context, Keyword::ARGUMENT_SEPARATOR, CompilationError::ARGUMENT_SEPARATOR_EXPECTED);
    
    if (!separator) {
        context->errorBuffer.currentIndex--;
        return ArgumentSeparator (parameter, NULL);
    }
    
    Tree::Node <AstNode> *nextParameter = GetParameterList (context, localNameTableId);
    NotNull (nextParameter);

    separator->left   = parameter;
    parameter->parent = separator;

    separator->right      = nextParameter;
    nextParameter->parent = separator;
    
    return separator;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------


