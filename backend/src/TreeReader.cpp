#include <memory_resource>
#include <stddef.h>
#include <ctype.h>

#include "BackendCore.h"
#include "CustomAssert.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeReader.h"

#define SkipSpaces()                            \
    do {                                        \
        while (isspace ((*treeBegin) [0])) {    \
            (*treeBegin)++;                     \
        }                                       \
    } while (0)

typedef TranslationError (* reader_t) (TranslationContext *context, char **dataString, Tree::Node <AstNode> *newNode);

static Tree::Node <AstNode> *ReadSyntaxTreeInternal (TranslationContext *context, char **treeBegin);

static int FindIdentifier (TranslationContext *context, const char *identifier);

static TranslationError ReadConstant    (TranslationContext *context, char **dataString, Tree::Node <AstNode> *newNode);
static TranslationError ReadKeyword     (TranslationContext *context, char **dataString, Tree::Node <AstNode> *newNode);
static TranslationError ReadName        (TranslationContext *context, char **dataString, Tree::Node <AstNode> *newNode);
static TranslationError DummyRead       (TranslationContext *context, char **dataString, Tree::Node <AstNode> *newNode);

static const reader_t Readers [7] = {ReadConstant, ReadName, ReadKeyword, ReadName, DummyRead, ReadName, DummyRead};

TranslationError ReadSyntaxTree (TranslationContext *context, char *fileContent) {
    PushLog (3);

    custom_assert (context,     pointer_is_null, TranslationError::CONTEXT_ERROR);
    custom_assert (fileContent, pointer_is_null, TranslationError::INPUT_FILE_ERROR);

    context->abstractSyntaxTree.root = ReadSyntaxTreeInternal (context, &fileContent);

    if (!context->abstractSyntaxTree.root) {
        RETURN TranslationError::INPUT_FILE_ERROR;
    }

    RETURN TranslationError::NO_ERRORS;
}

static Tree::Node <AstNode> *ReadSyntaxTreeInternal (TranslationContext *context, char **treeBegin) {
    PushLog (3);

    SkipSpaces ();

    if ((*treeBegin) [0] != '(') {
        (*treeBegin)++;
        RETURN NULL;
    }

    (*treeBegin)++;
    SkipSpaces ();

    int intNodeType = -1;
    sscanf (*treeBegin, "%d", &intNodeType);

    if (intNodeType < 1 || intNodeType > 7) {
        RETURN NULL;
    }
    
    (*treeBegin)++;
    SkipSpaces ();

    Tree::Node <AstNode> *newNode = NULL;
    Tree::InitNode (&newNode);

    newNode->nodeData.type = (NodeType) intNodeType;
    
    Readers [intNodeType - 1] (context, treeBegin, newNode);

    newNode->left  = ReadSyntaxTreeInternal (context, treeBegin);
    newNode->right = ReadSyntaxTreeInternal (context, treeBegin);

    if (newNode->left)
        newNode->left->parent  = newNode;

    if (newNode->right)
        newNode->right->parent = newNode;

    SkipSpaces ();
    (*treeBegin)++;

    RETURN newNode;
}

static TranslationError ReadConstant (TranslationContext *context, char **dataString, Tree::Node <AstNode> *newNode) {
    PushLog (4);
    
    double constant = NAN;
    int    length   = 0;
    sscanf (*dataString, "%lf%n", &constant, &length);

    (*dataString) += length;

    newNode->nodeData.content.number = constant;

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError ReadKeyword (TranslationContext *context, char **dataString, Tree::Node <AstNode> *newNode) {
    PushLog (4);
    
    int keywordIndex = NAN;
    int length   = 0;
    sscanf (*dataString, "%d%n", &keywordIndex, &length);

    (*dataString) += length;

    newNode->nodeData.content.keyword = (Keyword) keywordIndex;    

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError ReadName (TranslationContext *context, char **dataString, Tree::Node <AstNode> *newNode) {
    PushLog (4);

    if ((*dataString) [0] != '\"') {
        RETURN TranslationError::INPUT_FILE_ERROR;
    }
    
    (*dataString)++;
    
    char *newIdentifier  = NULL;
    int identifierLength = 0; 
    sscanf (*dataString, "%ms%n", &newIdentifier, &identifierLength);

    (*dataString) += identifierLength;

    newIdentifier [identifierLength - 1] = '\0';

    int identifierIndex = FindIdentifier (context, newIdentifier);

    if (identifierIndex == -1) {
        AddIdentifier (&context->nameTable, newIdentifier);
        newNode->nodeData.content.nameTableIndex = context->nameTable.currentIndex - 1;
    } else {
        
        newNode->nodeData.content.nameTableIndex = (size_t) identifierIndex;
        free (newIdentifier);
    }

    RETURN TranslationError::NO_ERRORS;
}

static int FindIdentifier (TranslationContext *context, const char *identifier) {
    PushLog (4);

    for (size_t identifierIndex = 0; identifierIndex < context->nameTable.currentIndex; identifierIndex++) {
        if (strcmp (context->nameTable.data [identifierIndex].name, identifier) == 0) {
            RETURN (int) identifierIndex;  
        }
    }
    
    RETURN -1;
}

static TranslationError DummyRead (TranslationContext *context, char **dataString, Tree::Node <AstNode> *newNode) {
    
    return TranslationError::NO_ERRORS;
}
