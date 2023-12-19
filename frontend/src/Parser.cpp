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
static Tree::Node <AstNode> *GetFunctionDefinition    (CompilationContext *context);
static Tree::Node <AstNode> *GetDeclaration           (CompilationContext *context);
static Tree::Node <AstNode> *GetOperator              (CompilationContext *context);
static Tree::Node <AstNode> *GetInitializerDeclarator (CompilationContext *context);
static Tree::Node <AstNode> *GetAssignmentExpression  (CompilationContext *context);
static Tree::Node <AstNode> *GetConditionOperator     (CompilationContext *context, Keyword operatorKeyword, CompilationError error);
static Tree::Node <AstNode> *GetOperatorList          (CompilationContext *context);


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

    NotNull (GetDestroyableToken(context, Keyword::INITIAL_OPERATOR, CompilationError::INITIAL_OPERATOR_EXPECTED));
    
    Tree::Node <AstNode> *rootNode = GetTranslationUnit (context);
    DestroyCurrentNode ();

    RETURN rootNode;
}

static Tree::Node <AstNode> *GetTranslationUnit (CompilationContext *context) {
    PushLog (2);

    Tree::Node <AstNode> *externalDeclaration = GetExternalDeclaration (context);

    NotNull (externalDeclaration);

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

    Tree::Node <AstNode> *root = GetFunctionDefinition (context);

    if (root) {
        RETURN root;
    }

    CheckForError (root, CompilationError::FUNCTION_EXPECTED);

    root = GetDeclaration (context);

    RETURN root;
}

static Tree::Node <AstNode> *GetFunctionDefinition (CompilationContext *context) {
    PushLog (2);

    NotNull (GetDestroyableToken (context, Keyword::FUNCTION_DEFINITION, CompilationError::FUNCTION_EXPECTED));

    Tree::Node <AstNode> *type = GetNameWithType (context, NameType::TYPE_NAME, CompilationError::TYPE_NAME_EXPECTED);
    NotNull (type);

    Tree::Node <AstNode> *identifier = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);

    size_t identifierIndex = identifier->nodeData.content.nameTableIndex;

    context->tokenIndex--;
    DestroyCurrentNode ();
    context->tokenIndex++;

    NotNull (GetDestroyableToken (context, Keyword::LBRACKET,   CompilationError::BRACKET_EXPECTED));
    NotNull (GetDestroyableToken (context, Keyword::RBRACKET,   CompilationError::BRACKET_EXPECTED));
    NotNull (GetDestroyableToken (context, Keyword::BLOCK_OPEN, CompilationError::CODE_BLOCK_EXPECTED));

    Tree::Node <AstNode> *functionContent = GetOperatorList (context);

    NotNull (GetDestroyableToken (context, Keyword::BLOCK_CLOSE, CompilationError::CODE_BLOCK_EXPECTED));

    RETURN FunctionDefinition (type, FunctionArguments (NULL, functionContent), identifierIndex);
}

static Tree::Node <AstNode> *GetDeclaration (CompilationContext *context) {
    PushLog (2);
   
    Tree::Node <AstNode> *type = GetNameWithType (context, NameType::TYPE_NAME, CompilationError::TYPE_NAME_EXPECTED);
    NotNull (type);

    Tree::Node <AstNode> *initializerDeclarator = GetInitializerDeclarator (context);
    NotNull (initializerDeclarator);

    size_t identifierIndex = 0;

    if (context->nameTable.data [initializerDeclarator->nodeData.content.nameTableIndex].type == NameType::IDENTIFIER) {
        identifierIndex = initializerDeclarator->nodeData.content.nameTableIndex;
    } else {
        identifierIndex = initializerDeclarator->right->nodeData.content.nameTableIndex;
    }

    RETURN VariableDeclaration (type, initializerDeclarator, identifierIndex);
}

static Tree::Node <AstNode> *GetInitializerDeclarator (CompilationContext *context) {
    PushLog (2);

    Tree::Node <AstNode> *initializer = GetAssignmentExpression (context);

    if (initializer) {
        RETURN initializer;
    }

    CheckForError (initializer, CompilationError::IDENTIFIER_EXPECTED);

    initializer = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);

    RETURN initializer;
}

static Tree::Node <AstNode> *GetAssignmentExpression (CompilationContext *context) {
    PushLog (2);

    Tree::Node <AstNode> *identifier = GetNameWithType (context, NameType::IDENTIFIER, CompilationError::IDENTIFIER_EXPECTED);
    NotNull (identifier);

    Tree::Node <AstNode> *assignmentOperation = GetKeyword (context, Keyword::ASSIGNMENT, CompilationError::ASSIGNMENT_EXPECTED);
    NotNull (assignmentOperation);

    Tree::Node <AstNode> *expression = GetExpression (context);
    NotNull (expression);

    assignmentOperation->left = expression;
    expression->parent        = assignmentOperation;

    assignmentOperation->right = identifier;
    identifier->parent         = assignmentOperation;

    RETURN assignmentOperation;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static Tree::Node <AstNode> *GetOperator (CompilationContext *context) {
    PushLog (2);

    Tree::Node <AstNode> *expectedOperator = NULL;

    expectedOperator = GetConditionOperator (context, Keyword::IF, CompilationError::IF_EXPECTED);
        CheckForError (expectedOperator, CompilationError::IF_EXPECTED);

        if (expectedOperator) {
            RETURN OperatorSeparator (expectedOperator, NULL);   
        }
        
        expectedOperator = GetConditionOperator (context, Keyword::WHILE, CompilationError::WHILE_EXPECTED);
        CheckForError (expectedOperator, CompilationError::WHILE_EXPECTED);
        
        if (expectedOperator) {
            RETURN OperatorSeparator (expectedOperator, NULL);   
        }

    while (true) {
        expectedOperator = GetAssignmentExpression (context);
        CheckForError (expectedOperator, CompilationError::IDENTIFIER_EXPECTED);

        if (expectedOperator)
            break;
        
        expectedOperator = GetDeclaration (context);
        CheckForError (expectedOperator, CompilationError::TYPE_NAME_EXPECTED);
            
        if (expectedOperator)
            break;

        NotNull (GetDestroyableToken (context, Keyword::BLOCK_OPEN,  CompilationError::CODE_BLOCK_EXPECTED));
        expectedOperator = GetOperatorList (context);
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

static Tree::Node <AstNode> *GetOperatorList (CompilationContext *context) {
    PushLog (2);
    
    Tree::Node <AstNode> *firstOperator  = GetOperator (context);
    NotNull (firstOperator);

    Tree::Node <AstNode> *secondOperator = GetOperatorList (context);

    firstOperator->right = secondOperator;

    if (!secondOperator) {
        context->errorList.currentIndex--;
    } else {
        secondOperator->parent = firstOperator;
    }

    RETURN firstOperator;
}

static Tree::Node <AstNode> *GetConditionOperator (CompilationContext *context, Keyword operatorKeyword, CompilationError error) {
    PushLog (2);

    Tree::Node <AstNode> *conditionOperator = GetKeyword (context, operatorKeyword, error);
    NotNull (conditionOperator);

    Tree::Node <AstNode> *conditionExpression = GetExpression (context);
    NotNull (conditionExpression);

    NotNull (GetDestroyableToken (context, Keyword::CONDITION_SEPARATOR, CompilationError::CONDITION_SEPARATOR_EXPECTED));

    Tree::Node <AstNode> *operatorContent = GetOperator (context);
    NotNull (operatorContent);

    conditionOperator->left     = conditionExpression;
    conditionExpression->parent = conditionOperator;

    conditionOperator->right = operatorContent;
    operatorContent->parent  = conditionOperator;

    RETURN conditionOperator;
}

