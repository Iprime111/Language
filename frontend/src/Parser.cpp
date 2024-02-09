#include <cstdio>
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

static Tree::Node <AstNode> *GetGrammar               (CompilationContext *context);
static Tree::Node <AstNode> *GetTranslationUnit       (CompilationContext *context);
static Tree::Node <AstNode> *GetExternalDeclaration   (CompilationContext *context);
static Tree::Node <AstNode> *GetFunctionDefinition    (CompilationContext *context, int localNameTable);
static Tree::Node <AstNode> *GetDeclaration           (CompilationContext *context, int localNameTable);
static Tree::Node <AstNode> *GetOperator              (CompilationContext *context, int localNameTable);
static Tree::Node <AstNode> *GetInitializerDeclarator (CompilationContext *context, int localNameTable);
static Tree::Node <AstNode> *GetAssignmentExpression  (CompilationContext *context, int localNameTable);
static Tree::Node <AstNode> *GetConditionOperator     (CompilationContext *context, Keyword operatorKeyword, CompilationError error, int localNameTable);
static Tree::Node <AstNode> *GetOperatorList          (CompilationContext *context, int localNameTable);
static Tree::Node <AstNode> *GetArgumentList          (CompilationContext *context, int localNameTable);
static Tree::Node <AstNode> *GetParameterList         (CompilationContext *context, int localNameTable);
static Tree::Node <AstNode> *GetReturnOperator        (CompilationContext *context, int localNameTable);
static Tree::Node <AstNode> *GetOutOperator           (CompilationContext *context, int localNameTable);

//---------------------------------------------------------------------------------------------------------------------------------------------------

CompilationError ParseCode (CompilationContext *context) {
    PushLog (1);

    context->tokenIndex = 0;
    context->abstractSyntaxTree.root = GetGrammar (context);

   RETURN context->error;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static Tree::Node <AstNode> *GetGrammar (CompilationContext *context) {
    PushLog (1);
    // TODO: GetAndDestroy ()
    NotNull (GetDestroyableToken (context, Keyword::INITIAL_OPERATOR, CompilationError::INITIAL_OPERATOR_EXPECTED));
   //TODO: GetStringToken () 
    Tree::Node <AstNode> *entryPointIdentifier = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (entryPointIdentifier);

    NotNull (GetDestroyableToken (context, Keyword::OPERATOR_SEPARATOR, CompilationError::OPERATOR_SEPARATOR_EXPECTED));

    context->entryPoint = entryPointIdentifier->nodeData.content.nameTableIndex;
    Tree::DestroySingleNode (entryPointIdentifier);
    
    Tree::Node <AstNode> *rootNode = GetTranslationUnit (context);
    DestroyCurrentNode ();
    
    //TODO: localNameTableId
    int localNameTable = 0;
    for (size_t callIndex = 0; callIndex < context->functionCalls.currentIndex; callIndex++) {
        DeclarationAssert (context->functionCalls.data [callIndex], LocalNameType::FUNCTION_IDENTIFIER, CompilationError::FUNCTION_NOT_DECLARED);
    }

    RETURN rootNode;
}

static Tree::Node <AstNode> *GetTranslationUnit (CompilationContext *context) {
    PushLog (2);

    Tree::Node <AstNode> *externalDeclaration = GetExternalDeclaration (context);

    NotNull (externalDeclaration);

    //TODO: STRING
    SyntaxAssert (currentToken->nodeData.type == NodeType::NAME && 
                    context->nameTable.data [currentNameTableIndex].keyword == Keyword::OPERATOR_SEPARATOR, 
                    CompilationError::OPERATOR_SEPARATOR_EXPECTED);

    currentToken->left          = externalDeclaration;
    externalDeclaration->parent = currentToken;

    Tree::Node <AstNode> *root = currentToken;

    context->tokenIndex++;

    if (currentToken->nodeData.type != NodeType::TERMINATOR) {
        root->right = GetTranslationUnit (context);
        
        if (root->right)
            root->right->parent = root;
    }

    RETURN root;
}

static Tree::Node <AstNode> *GetExternalDeclaration (CompilationContext *context) {
    PushLog (2);

    Tree::Node <AstNode> *root = GetFunctionDefinition (context, 0);

    if (root) {
        RETURN root;
    }

    CheckForError (root, CompilationError::FUNCTION_EXPECTED);

    root = GetDeclaration (context, 0);

    RETURN root;
}

static Tree::Node <AstNode> *GetFunctionDefinition (CompilationContext *context, int localNameTable) {
    PushLog (2);

    NotNull (GetDestroyableToken (context, Keyword::FUNCTION_DEFINITION, CompilationError::FUNCTION_EXPECTED));

    Tree::Node <AstNode> *type = GetNameWithType (context, NameType::TYPE_NAME, CompilationError::TYPE_NAME_EXPECTED);
    NotNull (type);

    Tree::Node <AstNode> *identifier = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
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

    NotNull (GetDestroyableToken (context, Keyword::LBRACKET,   CompilationError::BRACKET_EXPECTED));

    Tree::Node <AstNode> *parameters = GetParameterList (context, newNameTableIndex);
    CheckForError (parameters, CompilationError::TYPE_NAME_EXPECTED);

    NotNull (GetDestroyableToken (context, Keyword::RBRACKET,   CompilationError::BRACKET_EXPECTED));
    NotNull (GetDestroyableToken (context, Keyword::BLOCK_OPEN, CompilationError::CODE_BLOCK_EXPECTED));

    Tree::Node <AstNode> *functionContent = GetOperatorList (context, newNameTableIndex);

    CheckForError (functionContent, CompilationError::OPERATOR_NOT_FOUND);

    NotNull (GetDestroyableToken (context, Keyword::BLOCK_CLOSE, CompilationError::CODE_BLOCK_EXPECTED));

    RETURN FunctionDefinition (type, FunctionArguments (parameters, functionContent), identifierIndex);
}

static Tree::Node <AstNode> *GetDeclaration (CompilationContext *context, int localNameTable) {
    PushLog (2);
   
    Tree::Node <AstNode> *type = GetNameWithType (context, NameType::TYPE_NAME, CompilationError::TYPE_NAME_EXPECTED);
    NotNull (type);

    Tree::Node <AstNode> *identifier = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);
    context->tokenIndex--;

    size_t identifierIndex = identifier->nodeData.content.nameTableIndex;

    RedeclarationAssert (identifier, 
                         (LocalNameType) ((int) LocalNameType::VARIABLE_IDENTIFIER | (int) LocalNameType::FUNCTION_IDENTIFIER), 
                         CompilationError::VARIABLE_REDECLARATION);

    AddLocalIdentifier (localNameTable, &context->localTables, 
                        LocalNameTableRecord {.nameType = LocalNameType::VARIABLE_IDENTIFIER, .globalNameId = identifierIndex}, 1);
    

    Tree::Node <AstNode> *initializerDeclarator = GetInitializerDeclarator (context, localNameTable);
    NotNull (initializerDeclarator);


    RETURN VariableDeclaration (type, initializerDeclarator, identifierIndex);
}

static Tree::Node <AstNode> *GetInitializerDeclarator (CompilationContext *context, int localNameTable) {
    PushLog (2);

    Tree::Node <AstNode> *initializer = GetAssignmentExpression (context, localNameTable);

    if (initializer) {
        RETURN initializer;
    }

    CheckForError (initializer, CompilationError::ASSIGNMENT_EXPECTED);
    context->tokenIndex--; 

    initializer = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);

    RETURN initializer;
}

static Tree::Node <AstNode> *GetAssignmentExpression (CompilationContext *context, int localNameTable) {
    PushLog (2);

    Tree::Node <AstNode> *identifier = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);
    DeclarationAssert (identifier, LocalNameType::VARIABLE_IDENTIFIER, CompilationError::VARIABLE_NOT_DECLARED);


    Tree::Node <AstNode> *assignmentOperation = GetKeyword (context, Keyword::ASSIGNMENT, CompilationError::ASSIGNMENT_EXPECTED);
    NotNull (assignmentOperation);

    Tree::Node <AstNode> *expression = GetExpression (context, localNameTable);
    NotNull (expression);

    assignmentOperation->left = expression;
    expression->parent        = assignmentOperation;

    assignmentOperation->right = identifier;
    identifier->parent         = assignmentOperation;

    RETURN assignmentOperation;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static Tree::Node <AstNode> *GetOperator (CompilationContext *context, int localNameTable) {
    PushLog (2);

    Tree::Node <AstNode> *expectedOperator = NULL;

    expectedOperator = GetConditionOperator (context, Keyword::IF, CompilationError::IF_EXPECTED, localNameTable);
    CheckForError (expectedOperator, CompilationError::IF_EXPECTED);

    if (expectedOperator) {
        RETURN OperatorSeparator (expectedOperator, NULL);   
    }
    
    expectedOperator = GetConditionOperator (context, Keyword::WHILE, CompilationError::WHILE_EXPECTED, localNameTable);
    CheckForError (expectedOperator, CompilationError::WHILE_EXPECTED);
    
    if (expectedOperator) {
        RETURN OperatorSeparator (expectedOperator, NULL);   
    }

    do {
// TODO: MORE LOCAL DEFINES TO GOD OF DEFINES!
        expectedOperator = GetKeyword (context, Keyword::ABORT, CompilationError::ABORT_EXPECTED);
        TryGetOperator (ABORT_EXPECTED);

        expectedOperator = GetOutOperator (context, localNameTable);
        TryGetOperator (OUT_EXPECTED);

        expectedOperator = GetKeyword (context, Keyword::BREAK_OPERATOR, CompilationError::BREAK_EXPECTED);
        TryGetOperator (BREAK_EXPECTED);

        expectedOperator = GetKeyword (context, Keyword::CONTINUE_OPERATOR, CompilationError::CONTINUE_EXPECTED);
        TryGetOperator (CONTINUE_EXPECTED);

        expectedOperator = GetReturnOperator (context, localNameTable);
        TryGetOperator (RETURN_EXPECTED);

        expectedOperator = GetFunctionCall (context, localNameTable);
        TryGetOperator (FUNCTION_CALL_EXPECTED);

        expectedOperator = GetAssignmentExpression (context, localNameTable);
        TryGetOperator (IDENTIFIER_EXPECTED);
        
        expectedOperator = GetDeclaration (context, localNameTable);
        TryGetOperator (TYPE_NAME_EXPECTED);

        NotNull (GetDestroyableToken (context, Keyword::BLOCK_OPEN,  CompilationError::OPERATOR_NOT_FOUND));
        expectedOperator = GetOperatorList (context, localNameTable);
        NotNull (expectedOperator);
        NotNull (GetDestroyableToken (context, Keyword::BLOCK_CLOSE, CompilationError::OPERATOR_NOT_FOUND));

    } while (0);

    Tree::Node <AstNode> *separator = GetKeyword (context, Keyword::OPERATOR_SEPARATOR, CompilationError::OPERATOR_SEPARATOR_EXPECTED);
    NotNull (separator);

    separator->left          = expectedOperator;
    expectedOperator->parent = separator;

    RETURN separator;
}

static Tree::Node <AstNode> *GetOperatorList (CompilationContext *context, int localNameTable) {
    PushLog (2);
    
    Tree::Node <AstNode> *firstOperator  = GetOperator     (context, localNameTable);
    NotNull (firstOperator);

    Tree::Node <AstNode> *secondOperator = GetOperatorList (context, localNameTable);

    firstOperator->right = secondOperator;

    CheckForError (secondOperator, CompilationError::OPERATOR_NOT_FOUND);

    if (secondOperator)
        secondOperator->parent = firstOperator;


    RETURN firstOperator;
}

static Tree::Node <AstNode> *GetConditionOperator (CompilationContext *context, Keyword operatorKeyword, CompilationError error, int localNameTable) {
    PushLog (2);

    Tree::Node <AstNode> *conditionOperator = GetKeyword (context, operatorKeyword, error);
    NotNull (conditionOperator);

    Tree::Node <AstNode> *conditionExpression = GetExpression (context, localNameTable);
    NotNull (conditionExpression);

    NotNull (GetDestroyableToken (context, Keyword::CONDITION_SEPARATOR, CompilationError::CONDITION_SEPARATOR_EXPECTED));

    Tree::Node <AstNode> *operatorContent = GetOperator (context, localNameTable);
    NotNull (operatorContent);

    conditionOperator->left     = conditionExpression;
    conditionExpression->parent = conditionOperator;

    conditionOperator->right = operatorContent;
    operatorContent->parent  = conditionOperator;

    RETURN conditionOperator;
}

static Tree::Node <AstNode> *GetReturnOperator (CompilationContext *context, int localNameTable) {
    PushLog (2);

    Tree::Node <AstNode> *returnStatement = GetKeyword (context, Keyword::RETURN_OPERATOR, CompilationError::RETURN_EXPECTED);
    NotNull (returnStatement);

    Tree::Node <AstNode> *expression = GetExpression (context, localNameTable);
    NotNull (expression);

    returnStatement->right = expression;
    expression->parent     = returnStatement;

    RETURN returnStatement;
}

static Tree::Node <AstNode> *GetOutOperator (CompilationContext *context, int localNameTable) {
    PushLog (2);

    Tree::Node <AstNode> *outOperator = GetKeyword (context, Keyword::OUT, CompilationError::OUT_EXPECTED);
    NotNull (outOperator);

    Tree::Node <AstNode> *expression = GetExpression (context, localNameTable);
    NotNull (expression);

    outOperator->right = expression;
    expression->parent = outOperator;

    RETURN outOperator;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

Tree::Node <AstNode> *GetFunctionCall (CompilationContext *context, int localNameTable) {
    PushLog (2);

    NotNull (GetDestroyableToken (context, Keyword::FUNCTION_CALL, CompilationError::FUNCTION_CALL_EXPECTED));

    Tree::Node <AstNode> *identifier = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);
    WriteDataToBuffer (&context->functionCalls, &identifier, 1);

    NotNull (GetDestroyableToken (context, Keyword::LBRACKET, CompilationError::BRACKET_EXPECTED));

    Tree::Node <AstNode> *arguments = GetArgumentList (context, localNameTable);
    CheckForError (arguments, CompilationError::CONSTANT_EXPECTED);

    NotNull (GetDestroyableToken (context, Keyword::RBRACKET, CompilationError::BRACKET_EXPECTED));

    RETURN FunctionCall (arguments, identifier);
}

static Tree::Node <AstNode> *GetArgumentList (CompilationContext *context, int localNameTable) {
    PushLog (2);

    Tree::Node <AstNode> *argument = GetExpression (context, localNameTable);
    NotNull (argument);

    Tree::Node <AstNode> *separator = GetKeyword (context, Keyword::ARGUMENT_SEPARATOR, CompilationError::ARGUMENT_SEPARATOR_EXPECTED);
    
    if (!separator) {
        context->errorList.currentIndex--;
        RETURN ArgumentSeparator (argument, NULL);
    }
    
    Tree::Node <AstNode> *nextArgument = GetArgumentList (context, localNameTable);
    NotNull (nextArgument);

    separator->left  = argument;
    argument->parent = separator;

    separator->right     = nextArgument;
    nextArgument->parent = separator;
    
    RETURN separator;
}

static Tree::Node <AstNode> *GetParameterList (CompilationContext *context, int localNameTable) {
     PushLog (2);

    Tree::Node <AstNode> *parameter = GetDeclaration (context, localNameTable);
    NotNull (parameter);

    Tree::Node <AstNode> *separator = GetKeyword (context, Keyword::ARGUMENT_SEPARATOR, CompilationError::ARGUMENT_SEPARATOR_EXPECTED);
    
    if (!separator) {
        // TODO: Do not name not lists with list
        context->errorList.currentIndex--;
        RETURN ArgumentSeparator (parameter, NULL);
    }
    
    Tree::Node <AstNode> *nextParameter = GetParameterList (context, localNameTable);
    NotNull (nextParameter);

    separator->left   = parameter;
    parameter->parent = separator;

    separator->right      = nextParameter;
    nextParameter->parent = separator;
    
    RETURN separator;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------


