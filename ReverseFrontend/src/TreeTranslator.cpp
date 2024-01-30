
#include "TreeTranslator.h"
#include "Buffer.h"
#include "Logger.h"
#include "NameTable.h"
#include "ReverseFrontendCore.h"
#include "SyntaxTree.h"
#include "TreeReader.h"

static TranslationError TreeTraversal (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);

static TranslationError WriteIdentifier (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);
static TranslationError WriteConstant   (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);
static TranslationError WriteKeyword    (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);

static const char *GetKeyword (Keyword keyword);

static const char *functionDefinition = GetKeyword (Keyword::FUNCTION_DEFINITION);
static const char *blockOpen          = GetKeyword (Keyword::BLOCK_OPEN);
static const char *blockClose         = GetKeyword (Keyword::BLOCK_CLOSE);
static const char *conditionSeparator = GetKeyword (Keyword::CONDITION_SEPARATOR);

TranslationError TranslateTree (TranslationContext *context, FILE *stream) {
    PushLog (1);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN TranslationError::OUTPUT_FILE_ERROR;
    }

    TreeTraversal (context, context->abstractSyntaxTree.root, &outputBuffer);

    outputBuffer.data [outputBuffer.currentIndex] = '\0';
    printf ("%s", outputBuffer.data);

    DestroyBuffer (&outputBuffer);

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError TreeTraversal (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    PushLog (2);

    if (!node) {
        RETURN TranslationError::NO_ERRORS;
    }

    switch (node->nodeData.type) {

        case NodeType::TERMINATOR: { RETURN TranslationError::TREE_ERROR; }
        case NodeType::CONSTANT:   { WriteWordNode (WriteConstant);   break;}
        case NodeType::NAME:       { WriteWordNode (WriteIdentifier); break;}
        case NodeType::KEYWORD:    {WriteWordNode (WriteKeyword); break;}

        case NodeType::FUNCTION_DEFINITION: {
            Traversal (left); 
            WriteString (functionDefinition);
            WriteString (" "); WriteWordNode (WriteIdentifier); 
            Traversal (right); 
            break;
        }

        case NodeType::FUNCTION_ARGUMENTS:  {
            WriteString ("("); 
            Traversal   (left); 
            WriteString (") ");
            WriteString (blockOpen);
            WriteString ("\n");
            Traversal   (right);
            WriteString ("\n");
            WriteString (blockClose);
            break;
        }

        case NodeType::VARIABLE_DECLARATION: {Traversal (left); Traversal (right); break;}
        case NodeType::FUNCTION_CALL:
          break;
    }

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError WriteIdentifier (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    PushLog (4);

    WriteString (context->nameTable.data [node->nodeData.content.nameTableIndex].name);

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError WriteConstant (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    PushLog (4);

    char numberBuffer [MAX_NUMBER_LENGTH] = "";

    snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%lg", node->nodeData.content.number);
    WriteString (numberBuffer);

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError WriteKeyword (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    PushLog (4);

    const char *keyword = GetKeyword (node->nodeData.content.keyword);

    switch (node->nodeData.content.keyword) {

        case Keyword::OPERATOR_SEPARATOR: { Traversal (left); WriteString (keyword); WriteString ("\n"); Traversal (right); break;}
        case Keyword::ARGUMENT_SEPARATOR: { Traversal (left); WriteString (keyword); WriteString (" ");  Traversal (right); break;}
        case Keyword::WHILE:
        case Keyword::IF: {
            WriteString (keyword);
            Traversal (left);
            WriteString (conditionSeparator);
            WriteString (blockOpen);
            WriteString ("\n");
            Traversal (right);
            WriteString (blockClose);
            break;
        }

        case Keyword::ASSIGNMENT: { Traversal (right); WriteString (" "); WriteString (keyword); WriteString (" "); Traversal (left); break;}

        case Keyword::NUMBER:
        case Keyword::RETURN_OPERATOR:
        case Keyword::BREAK_OPERATOR:
        case Keyword::CONTINUE_OPERATOR:
        case Keyword::ABORT:
        case Keyword::SIN:
        case Keyword::COS:
        case Keyword::FLOOR:
        case Keyword::ADD:
        case Keyword::SUB:
        case Keyword::MUL:
        case Keyword::DIV:
        case Keyword::EQUAL:
        case Keyword::LESS:
        case Keyword::GREATER:
        case Keyword::LESS_EQUAL:
        case Keyword::GREATER_EQUAL:
        case Keyword::NOT_EQUAL:
        case Keyword::AND:
        case Keyword::OR:
        case Keyword::NOT:
        case Keyword::IN:
        case Keyword::OUT: { Traversal (left); WriteString (" "); WriteString (keyword); WriteString (" "); Traversal (right); break;}
    
        default:
            break;
    }

    RETURN TranslationError::NO_ERRORS;
}

static const char *GetKeyword (Keyword keyword) {

    #define KEYWORD(NAME, NUMBER, WORD, TYPE, ...)  \
        if (keyword == Keyword::NAME) {             \
            return WORD;                            \
        }

    #include "Keywords.def"

    #undef KEYWORD

    return NULL;
}
