#include <cstdio>
#include <cassert>

#include "TreeTranslator.h"
#include "Buffer.h"
#include "NameTable.h"
#include "ReverseFrontendCore.h"
#include "SyntaxTree.h"
#include "TreeDefinitions.h"
#include "TreeReader.h"

static TranslationError TreeTraversal (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);

static TranslationError WriteIdentifier          (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);
static TranslationError WriteConstant            (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);
static TranslationError WriteKeyword             (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);
static TranslationError WriteOperator            (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, const char *keyword);
static TranslationError WriteOperatorSeparator   (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, const char *keyword);
static TranslationError WriteConditionalOperator (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, const char *keyword);

static TranslationError WriteIndentation         (TranslationContext *context, Buffer <char> *outputBuffer);

static size_t            DeterminePriority       (Tree::Node <AstNode> *node);
static BracketsPlacement GetBracketsPlacement    (Tree::Node <AstNode> *node);

static const char *GetKeyword        (Keyword keyword);

static const char *functionDefinition = GetKeyword (Keyword::FUNCTION_DEFINITION);
static const char *blockOpen          = GetKeyword (Keyword::BLOCK_OPEN);
static const char *blockClose         = GetKeyword (Keyword::BLOCK_CLOSE);
static const char *conditionSeparator = GetKeyword (Keyword::CONDITION_SEPARATOR);
static const char *functionCall       = GetKeyword (Keyword::FUNCTION_CALL);
static const char *rBracket           = GetKeyword (Keyword::RBRACKET);
static const char *lBracket           = GetKeyword (Keyword::LBRACKET);
static const char *initialOperator    = GetKeyword (Keyword::INITIAL_OPERATOR);
static const char *operatorSeparator  = GetKeyword (Keyword::OPERATOR_SEPARATOR);

#define KeywordCase(WORD) case Keyword::WORD:

TranslationError TranslateTree (TranslationContext *context, FILE *stream) {
    assert (context);
    assert (stream);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::OUTPUT_FILE_ERROR;

    if (WriteStringToBuffer (&outputBuffer, initialOperator) != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::OUTPUT_FILE_ERROR;

    if (WriteStringToBuffer (&outputBuffer, " ") != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::OUTPUT_FILE_ERROR;

    if (WriteStringToBuffer (&outputBuffer, context->nameTable.data [context->entryPoint].name) != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::OUTPUT_FILE_ERROR;

    if (WriteStringToBuffer (&outputBuffer, operatorSeparator) != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::OUTPUT_FILE_ERROR;

    if (WriteStringToBuffer (&outputBuffer, "\n\n") != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::OUTPUT_FILE_ERROR;

    TreeTraversal (context, context->abstractSyntaxTree.root, &outputBuffer);

    fwrite (outputBuffer.data, outputBuffer.currentIndex, 1, stream);

    DestroyBuffer (&outputBuffer);

    return TranslationError::NO_ERRORS;
}

static TranslationError TreeTraversal (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    assert (context);
    assert (node);
    assert (outputBuffer);

    if (!node)
        return TranslationError::NO_ERRORS;

    switch (node->nodeData.type) {

        case NodeType::TERMINATOR: { return TranslationError::TREE_ERROR; }
        case NodeType::CONSTANT:   { CallbackFunction (WriteConstant);   break;}
        case NodeType::STRING:     { CallbackFunction (WriteIdentifier); break;}
        case NodeType::KEYWORD:    { CallbackFunction (WriteKeyword);    break;}

        case NodeType::FUNCTION_DEFINITION: {
            WriteString (functionDefinition);
            WriteString (" ");
            Traversal (left);
            CallbackFunction (WriteIdentifier); 
            Traversal (right); 
            break;
        }

        case NodeType::FUNCTION_ARGUMENTS:  {
            WriteString ("("); 
            Traversal   (left); 
            WriteString (") ");
            WriteString (blockOpen);
            WriteString ("\n");
            context->indentationLevel++;
            Traversal   (right);
            context->indentationLevel--;
            WriteString (blockClose);
            break;
        }

        case NodeType::VARIABLE_DECLARATION: {Traversal (left); Traversal (right); break;}
        case NodeType::FUNCTION_CALL: {
            WriteString (functionCall);
            WriteString (" ");
            Traversal   (right);
            WriteString (lBracket);
            Traversal   (left);
            WriteString (rBracket);
            break;
        }
    }

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteIdentifier (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    assert (context);
    assert (node);
    assert (outputBuffer);

    WriteString (context->nameTable.data [node->nodeData.content.nameTableIndex].name);

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteConstant (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    assert (context);
    assert (node);
    assert (outputBuffer);

    char numberBuffer [MAX_NUMBER_LENGTH] = "";

    snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%lg", node->nodeData.content.number);
    WriteString (numberBuffer);

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteKeyword (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    assert (context);
    assert (node);
    assert (outputBuffer);

    const char *keyword = GetKeyword (node->nodeData.content.keyword);

    switch (node->nodeData.content.keyword) {

        KeywordCase (OPERATOR_SEPARATOR) { WriteOperatorSeparator (context, node, outputBuffer, keyword); break; }
        KeywordCase (ARGUMENT_SEPARATOR) { Traversal (left); if (node->right) { WriteString (keyword); WriteString (" ");  Traversal (right); } break; }

        KeywordCase (WHILE)
        KeywordCase (IF)                 { WriteString ("\n"); WriteConditionalOperator (context, node, outputBuffer, keyword); break; }

        KeywordCase(ASSIGNMENT)          { Traversal (right); WriteString (" "); WriteString (keyword); WriteString (" "); Traversal (left); break; }

        KeywordCase (BREAK_OPERATOR)     KeywordCase (CONTINUE_OPERATOR)  
        KeywordCase (ABORT)              { Indentation (); WriteString (keyword); break; }

        KeywordCase (NUMBER)             { WriteString (keyword); WriteString (" "); break; }

        KeywordCase (RETURN_OPERATOR)    KeywordCase (SIN)
        KeywordCase (COS)                KeywordCase (FLOOR)
        KeywordCase (ADD)                KeywordCase (SUB)
        KeywordCase (MUL)                KeywordCase (DIV)
        KeywordCase (EQUAL)              KeywordCase (LESS)
        KeywordCase (GREATER)            KeywordCase (LESS_EQUAL)
        KeywordCase (GREATER_EQUAL)      KeywordCase (NOT_EQUAL)
        KeywordCase (AND)                KeywordCase (OR)
        KeywordCase (NOT)                KeywordCase (IN)
        KeywordCase (OUT)                {  WriteOperator (context, node, outputBuffer, keyword); break; }
    
        default: break;
    }

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteConditionalOperator (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, const char *keyword) {
    assert (context);
    assert (node);
    assert (outputBuffer);
    assert (keyword);

    Indentation ();
    WriteString (keyword);
    WriteString (" ");
    Traversal (left);
    WriteString (" ");
    WriteString (conditionSeparator);
    WriteString (" ");
    WriteString (blockOpen);
    WriteString ("\n");

    context->indentationLevel++;
    Traversal (right);
    context->indentationLevel--;
    Indentation ();
    WriteString (blockClose);

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteOperatorSeparator (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, const char *keyword) {
    assert (context);
    assert (node);
    assert (outputBuffer);
    assert (keyword);

    bool isOperatorSeparator = false;
    if (node->left) {
        isOperatorSeparator = node->left->nodeData.type == NodeType::KEYWORD && 
                              node->left->nodeData.content.keyword == Keyword::OPERATOR_SEPARATOR;
    }

    bool isValuable = node->left && (!isOperatorSeparator);

    if (isValuable) { 
        Indentation ();
    }

    Traversal (left);

    if (isValuable) {
        WriteString (keyword);  
        WriteString ("\n\n");
    }

    Traversal (right);

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteOperator (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, const char *keyword) {
    assert (context);
    assert (node);
    assert (outputBuffer);
    assert (keyword);

    BracketsPlacement brackets = GetBracketsPlacement (node);
    bool leftBrackets  = (int) brackets & (int) BracketsPlacement::LEFT;
    bool rightBrackets = (int) brackets & (int) BracketsPlacement::RIGHT; 


    if (node->left) { 
        if (leftBrackets)
            WriteString (lBracket);

        Traversal (left); 

        if (leftBrackets)
            WriteString (rBracket);

        WriteString (" ");
    }
    WriteString (keyword);
    if (node->right) { 
        WriteString (" "); 

        if (rightBrackets)
            WriteString (lBracket);

        Traversal (right);

        if (rightBrackets)
            WriteString (rBracket);

    }

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteIndentation (TranslationContext *context, Buffer <char> *outputBuffer) {
    assert (context);
    assert (outputBuffer);

    for (size_t tab = 0; tab < context->indentationLevel; tab++)
        WriteString ("\t");

    return TranslationError::NO_ERRORS;
}

static const char *GetKeyword (Keyword keyword) {

    #define KEYWORD(NAME, NUMBER, WORD, TYPE, ...)  \
        if (keyword == Keyword::NAME) {             \
            return WORD;                            \
        }

    #include "Keywords.def"

    #undef KEYWORD

    return nullptr;
}

static BracketsPlacement GetBracketsPlacement (Tree::Node <AstNode> *node) {
    assert (node);

    size_t operationPriority = DeterminePriority (node);
    size_t rightPriority     = DeterminePriority (node->right);

    BracketsPlacement result = BracketsPlacement::NO_BRACKETS;

    if (node->left && operationPriority < DeterminePriority (node->left))
        result = (BracketsPlacement) ((int) result | (int) BracketsPlacement::LEFT);

    if (operationPriority < rightPriority)
        result = (BracketsPlacement) ((int) result | (int) BracketsPlacement::RIGHT);

    Keyword keyword = node->nodeData.content.keyword;

    if (operationPriority == rightPriority && 
        (keyword == Keyword::SUB || keyword == Keyword::DIV || keyword == Keyword::NOT_EQUAL || keyword == Keyword::LESS ||
         keyword == Keyword::GREATER || keyword == Keyword::LESS_EQUAL || keyword == Keyword::GREATER_EQUAL || keyword == Keyword::EQUAL)) {
        
        result = (BracketsPlacement) ((int) result | (int) BracketsPlacement::RIGHT);
    }

    return result;
}

static size_t DeterminePriority (Tree::Node <AstNode> *node) {
    assert (node);

    if (!node)
        return 0;

    switch (node->nodeData.content.keyword) {
        KeywordCase (IN)                                    { return 0; }

        KeywordCase (SIN)     KeywordCase (COS)
        KeywordCase (FLOOR)   KeywordCase (NOT)             { return 1; }

        KeywordCase (MUL)     KeywordCase (DIV)             { return 2; }

        KeywordCase (SUB)     KeywordCase (ADD)             { return 3; }

        KeywordCase (EQUAL)   KeywordCase (NOT_EQUAL)
        KeywordCase (LESS)    KeywordCase (LESS_EQUAL)
        KeywordCase (GREATER) KeywordCase (GREATER_EQUAL)   { return 4; }

        KeywordCase (AND)     KeywordCase (OR)              { return 5; }

        KeywordCase (OUT)     KeywordCase (RETURN_OPERATOR) { return 6; }

        default: { return 0; }
    }
}
