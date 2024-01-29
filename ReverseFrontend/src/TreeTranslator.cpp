
#include "TreeTranslator.h"
#include "Logger.h"
#include "ReverseFrontendCore.h"
#include "SyntaxTree.h"
#include "TreeReader.h"

static TranslationError TreeTraversal (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);

static TranslationError WriteIdentifier (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);
static TranslationError WriteConstant   (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);
static TranslationError WriteKeyword    (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);

TranslationError TranslateTree (TranslationContext *context, FILE *stream) {
    PushLog (1);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN TranslationError::OUTPUT_FILE_ERROR;
    }

    TreeTraversal (context, context->abstractSyntaxTree.root, &outputBuffer);

    outputBuffer.data [outputBuffer.currentIndex] = '\0';
    printf ("%s", outputBuffer.data);

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
        case NodeType::KEYWORD:    { Traversal (left); WriteWordNode (WriteKeyword); Traversal (right); break;}

        case NodeType::FUNCTION_DEFINITION:  {Traversal (left); WriteString ("Физтех-школа "); WriteWordNode (WriteIdentifier); Traversal (right); break;}
        case NodeType::FUNCTION_ARGUMENTS: {
            WriteString ("("); 
            Traversal (left); 
            WriteString (") заебало уже\n"); 
            Traversal (right);
            WriteString ("каждый раз это.\n");
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

    snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%lf", node->nodeData.content.number);
    WriteString (numberBuffer);

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError WriteKeyword (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    PushLog (4);

    #define KEYWORD(NAME, NUMBER, WORD, TYPE, ...)              \
        if (node->nodeData.content.keyword == Keyword::NAME) {  \
            WriteString (WORD);                                 \
        }

    #include "Keywords.def"

    #undef KEYWORD

    RETURN TranslationError::NO_ERRORS;
}

