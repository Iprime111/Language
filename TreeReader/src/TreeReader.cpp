#include <cctype>
#include <cassert>

#include "TreeReader.h"
#include "SyntaxTree.h"

#define SkipSpaces()                            \
    do {                                        \
        while (isspace ((*treeBegin) [0])) {    \
            (*treeBegin)++;                     \
        }                                       \
    } while (0)

typedef TranslationError (* reader_t) (char **dataString, Tree::Node <AstNode> *newNode);

static Tree::Node <AstNode> *ReadSyntaxTreeInternal (Tree::Tree <AstNode> *tree, char **treeBegin);

static TranslationError ReadConstant (char **dataString, Tree::Node <AstNode> *newNode);
static TranslationError ReadKeyword  (char **dataString, Tree::Node <AstNode> *newNode);
static TranslationError ReadName     (char **dataString, Tree::Node <AstNode> *newNode);
static TranslationError DummyRead    (char **dataString, Tree::Node <AstNode> *newNode);

static const reader_t Readers [7] = {ReadConstant, ReadName, ReadKeyword, ReadName, DummyRead, ReadName, DummyRead};

TranslationError ReadSyntaxTree (Tree::Tree <AstNode> *tree, size_t *entryPoint, char *fileContent) {
    assert (tree);
    assert (fileContent);

    int entryPointLength = 0;
    sscanf (fileContent, "%lu%n", entryPoint, &entryPointLength);

    fileContent += entryPointLength;

    tree->root = ReadSyntaxTreeInternal (tree, &fileContent);

    if (!tree->root) {
        return TranslationError::INPUT_FILE_ERROR;
    }

    return TranslationError::NO_ERRORS;
}

static Tree::Node <AstNode> *ReadSyntaxTreeInternal (Tree::Tree <AstNode> *tree, char **treeBegin) {
    assert (tree);
    assert (treeBegin);

    SkipSpaces ();

    if ((*treeBegin) [0] != '(') {
        (*treeBegin)++;
        return nullptr;
    }

    (*treeBegin)++;
    SkipSpaces ();

    int intNodeType = -1;
    sscanf (*treeBegin, "%d", &intNodeType);

    if (intNodeType < 1 || intNodeType > 7) {
        return nullptr;
    }
    
    (*treeBegin)++;
    SkipSpaces ();

    Tree::Node <AstNode> *newNode = nullptr;
    Tree::InitNode (&newNode);

    newNode->nodeData.type = (NodeType) intNodeType;
    
    Readers [intNodeType - 1] (treeBegin, newNode);

    newNode->left  = ReadSyntaxTreeInternal (tree, treeBegin);
    newNode->right = ReadSyntaxTreeInternal (tree, treeBegin);

    if (newNode->left)
        newNode->left->parent  = newNode;

    if (newNode->right)
        newNode->right->parent = newNode;

    SkipSpaces ();
    (*treeBegin)++;

    return newNode;
}

static TranslationError ReadConstant (char **dataString, Tree::Node <AstNode> *newNode) {
    assert (dataString);
    assert (newNode);
    
    double constant = NAN;
    int    length   = 0;
    sscanf (*dataString, "%lf%n", &constant, &length);

    (*dataString) += length;

    newNode->nodeData.content.number = constant;

    return TranslationError::NO_ERRORS;
}

static TranslationError ReadKeyword (char **dataString, Tree::Node <AstNode> *newNode) {
    assert (dataString);
    assert (newNode);
    
    int keywordIndex = 0;
    int length   = 0;
    sscanf (*dataString, "%d%n", &keywordIndex, &length);

    (*dataString) += length;

    newNode->nodeData.content.keyword = (Keyword) keywordIndex;    

    return TranslationError::NO_ERRORS;
}

static TranslationError ReadName (char **dataString, Tree::Node <AstNode> *newNode) {
    assert (dataString);
    assert (newNode);

    int identifierLength = 0; 
    sscanf (*dataString, "%lu%n", &newNode->nodeData.content.nameTableIndex, &identifierLength);

    (*dataString) += identifierLength;

    return TranslationError::NO_ERRORS;
}

static TranslationError DummyRead (char **dataString, Tree::Node <AstNode> *newNode) {
    assert (dataString);
    assert (newNode);

    return TranslationError::NO_ERRORS;
}
