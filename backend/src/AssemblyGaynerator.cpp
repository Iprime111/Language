#include <cstdio>

#include "AssemblyGaynerator.h"
#include "BackendCore.h"
#include "Buffer.h"
#include "CustomAssert.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"

#define WriteString(data)                                                                       \
    do {                                                                                        \
        if (WriteStringToBuffer (outputBuffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {    \
            RETURN TranslationError::OUTPUT_FILE_ERROR;                                         \
        }                                                                                       \
    } while (0)

static TranslationError SetRbp (Buffer <char> *outputBuffer);

static TranslationError TreeTraversal (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);

static TranslationError WriteConstant             (double constant, Buffer <char> *outputBuffer);
static TranslationError WriteIdentifier           (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError NewFunction               (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError NewVariable               (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError WriteFunctionCall         (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError WriteKeyword              (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError WriteIdentifierMemoryCell (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError WriteLabel                (TranslationContext *context, Buffer <char> *outputBuffer, size_t labelIndex);

TranslationError GenerateAssembly (TranslationContext *context, FILE *outputStream) {
    PushLog (1);

    custom_assert (context, pointer_is_null, TranslationError::CONTEXT_ERROR);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN TranslationError::OUTPUT_FILE_ERROR;
    }

    SetRbp (&outputBuffer);
    TreeTraversal (context, context->abstractSyntaxTree.root, &outputBuffer, 0);

    fwrite (outputBuffer.data, outputBuffer.currentIndex, 1, outputStream);

    DestroyBuffer (&outputBuffer);

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError SetRbp (Buffer <char> *outputBuffer) {
    PushLog (4);

    char initialAddressBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (initialAddressBuffer, MAX_NUMBER_LENGTH, "%d", INITIAL_ADDRESS);

    WriteString ("\tpush ");
    WriteString (initialAddressBuffer);
    WriteString ("\n\tpop rbp\n");

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError TreeTraversal (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (2);

    if (!node) {
        RETURN TranslationError::NO_ERRORS;
    }

    #define NextCall(enumMember, function)                                  \
        case NodeType::enumMember: {                                        \
            function (context, node, outputBuffer, currentNameTableIndex);  \
            break;                                                          \
        }

    switch (node->nodeData.type) {

        case NodeType::TERMINATOR: {
            RETURN TranslationError::TREE_ERROR;
        }

        case NodeType::CONSTANT: {
            WriteConstant (node->nodeData.content.number, outputBuffer);
            break;
        }

        case NodeType::FUNCTION_ARGUMENTS: {
            context->areCallArguments = false;

            TreeTraversal (context, node->left,  outputBuffer, currentNameTableIndex);
            TreeTraversal (context, node->right, outputBuffer, currentNameTableIndex);
            break;
        }

        NextCall (NAME,                 WriteIdentifier);
        NextCall (KEYWORD,              WriteKeyword);
        NextCall (FUNCTION_DEFINITION,  NewFunction);
        NextCall (VARIABLE_DECLARATION, NewVariable);
        NextCall (FUNCTION_CALL,        WriteFunctionCall);
    }

    #undef NextCall

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError WriteConstant (double constant, Buffer <char> *outputBuffer) {
    PushLog (4);
    
    char numberBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lg", constant);

    WriteString ("\tpush ");
    WriteString (numberBuffer);
    WriteString ("\n");

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError WriteIdentifier (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (4);

    WriteString ("\tpush ");

    WriteIdentifierMemoryCell (context, node, outputBuffer, currentNameTableIndex);

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError WriteIdentifierMemoryCell (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (4);

    char indexBuffer [MAX_NUMBER_LENGTH] = "";
    int  identifierIndex                 = -1;

    if (currentNameTableIndex != 0) {
        identifierIndex = GetIndexInLocalTable (currentNameTableIndex, &context->localTables, node->nodeData.content.nameTableIndex, LocalNameType::VARIABLE_IDENTIFIER);
        
        if (identifierIndex >= 0) {
            snprintf (indexBuffer, MAX_NUMBER_LENGTH, "%d", identifierIndex);
        
            WriteString ("[rbp+");
            WriteString (indexBuffer);
            WriteString ("]\n");
        
            RETURN TranslationError::NO_ERRORS;
        }
    }

    identifierIndex = GetIndexInLocalTable (0, &context->localTables, node->nodeData.content.nameTableIndex, LocalNameType::VARIABLE_IDENTIFIER);

    if (identifierIndex < 0) {
        RETURN TranslationError::TREE_ERROR;
    }

    identifierIndex += INITIAL_ADDRESS;
    snprintf (indexBuffer, MAX_NUMBER_LENGTH, "%d", identifierIndex);

    WriteString ("[");
    WriteString (indexBuffer);
    WriteString ("]\n");

    RETURN TranslationError::NO_ERRORS;

}

static TranslationError NewFunction (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (4);

    WriteString (context->nameTable.data [node->nodeData.content.nameTableIndex].name);
    WriteString (":\n");

    int newTableIndex = AddLocalNameTable ((int) node->nodeData.content.nameTableIndex, &context->localTables);

    if (newTableIndex < 0) {
        RETURN TranslationError::NAME_TABLE_ERROR;
    }

    if (node->right) {
        TreeTraversal (context, node->right, outputBuffer, newTableIndex);
    } else {
        RETURN TranslationError::TREE_ERROR;
    }

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError NewVariable (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (4);

    //TODO: variable size
    AddLocalIdentifier (currentNameTableIndex, &context->localTables, 
                        LocalNameTableRecord {.nameType = LocalNameType::VARIABLE_IDENTIFIER, .globalNameId = node->nodeData.content.nameTableIndex}, 1);

    if (node->right && node->right->nodeData.type == NodeType::KEYWORD) {
        TreeTraversal (context, node->right, outputBuffer, currentNameTableIndex);
    }

    RETURN TranslationError::NO_ERRORS; 
}

static TranslationError WriteFunctionCall (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (4);

    if (!node->right) {
        RETURN TranslationError::TREE_ERROR;
    }

    context->areCallArguments = true;

    WriteString ("\tpush rbp\n");

    if (node->left) {
        TreeTraversal (context, node->left, outputBuffer, currentNameTableIndex);
    }

    char stackFrameSizeBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (stackFrameSizeBuffer, MAX_NUMBER_LENGTH, "%lu", context->localTables.data [currentNameTableIndex].tableSize);
    
    WriteString ("\tpush rbp+");
    WriteString (stackFrameSizeBuffer);
    WriteString ("\n\tpop rbp\n");
    
    WriteString ("\tcall ");
    WriteString (context->nameTable.data [node->right->nodeData.content.nameTableIndex].name);
    WriteString ("\n\tpop rbp\n");

    if (node->parent && !(node->parent->nodeData.type == NodeType::KEYWORD && node->parent->nodeData.content.keyword == Keyword::OPERATOR_SEPARATOR)) { 
        WriteString ("\tpush rax\n");
    }

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError WriteLabel (TranslationContext *context, Buffer <char> *outputBuffer, size_t labelIndex) {
    PushLog (4);
    
    char labelIndexBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (labelIndexBuffer, MAX_NUMBER_LENGTH, "%lu", labelIndex);

    WriteString ("L");
    WriteString (labelIndexBuffer);

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError WriteKeyword (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (4);

    #define AssemblyOperator(opcode, ...)   \
    case Keyword::opcode: {                 \
            __VA_ARGS__                     \
            break;                          \
        }

    switch (node->nodeData.content.keyword) {
        AssemblyOperator (IF, {

            Traversal (left);
            WriteString ("\tpush 0\n\tje ");
            size_t labelIndex = context->labelIndex++;
            Label (labelIndex);
            WriteString ("\n");
            Traversal (right);
            NewLabel (labelIndex);
        })

        AssemblyOperator (WHILE, {
            
            context->lastCycleBeginLabel = context->labelIndex++;
            NewLabel (context->lastCycleBeginLabel);
            Traversal (left);
            WriteString ("\tpush 0\n\tje ");
            context->lastCycleEndLabel = context->labelIndex++;
            Label (context->lastCycleEndLabel);
            WriteString ("\n");
            Traversal (right);
            WriteString ("\tjmp ");
            Label (context->lastCycleBeginLabel);
            WriteString ("\n");
            NewLabel (context->lastCycleEndLabel);
        })

        AssemblyOperator (ASSIGNMENT,         Traversal (left); WriteString ("\tpop "); MemoryCell (right);)
        AssemblyOperator (SIN,                UnaryOperation  ("sin"))
        AssemblyOperator (COS,                UnaryOperation  ("cos"))
        AssemblyOperator (FLOOR,              UnaryOperation  ("floor"))
        AssemblyOperator (ADD,                BinaryOperation ("add"))
        AssemblyOperator (SUB,                BinaryOperation ("sub"))
        AssemblyOperator (MUL,                BinaryOperation ("mul"))
        AssemblyOperator (DIV,                BinaryOperation ("div"))
        AssemblyOperator (EQUAL,              JumpOperator    ("je"))
        AssemblyOperator (LESS,               JumpOperator    ("jb"))
        AssemblyOperator (GREATER,            JumpOperator    ("ja"))
        AssemblyOperator (LESS_EQUAL,         JumpOperator    ("jbe"))
        AssemblyOperator (GREATER_EQUAL,      JumpOperator    ("jae"))
        AssemblyOperator (NOT_EQUAL,          JumpOperator    ("jne"))
        AssemblyOperator (AND,                LogicOperator   ("mul"))
        AssemblyOperator (OR,                 LogicOperator   ("add"))
        AssemblyOperator (ABORT,              WriteString     ("\thlt\n");)
        AssemblyOperator (NOT,                ToLogicExpression (left); LogicJump ("je");)
        AssemblyOperator (RETURN_OPERATOR,    Traversal (right); WriteString ("\tpop rax\n\tret\n");)
        AssemblyOperator (BREAK_OPERATOR,     WriteString ("\tjmp "); Label (context->lastCycleEndLabel);   WriteString ("\n");)
        AssemblyOperator (CONTINUE_OPERATOR,  WriteString ("\tjmp "); Label (context->lastCycleBeginLabel); WriteString ("\n");)
        AssemblyOperator (IN,                 WriteString ("\tin\n");)
        AssemblyOperator (OUT,                Traversal (right); WriteString ("\tout\n");)
        AssemblyOperator (OPERATOR_SEPARATOR, BothWayTraversal ();)
        AssemblyOperator (ARGUMENT_SEPARATOR, {
            if (context->areCallArguments) {
                BothWayTraversal ();
            } else {
                Traversal (right); 
                Traversal (left); 
                WriteString ("\tpop "); 
                MemoryCell (left); 
            }
        })

        case Keyword::FUNCTION_CALL:
        case Keyword::FUNCTION_DEFINITION:
        case Keyword::LBRACKET:
        case Keyword::RBRACKET:
        case Keyword::BLOCK_OPEN:
        case Keyword::BLOCK_CLOSE:
        case Keyword::CONDITION_SEPARATOR:
        case Keyword::INITIAL_OPERATOR:
        case Keyword::NOT_KEYWORD:
        case Keyword::NUMBER: {
            RETURN TranslationError::TREE_ERROR;
        }

    }

    #undef AssemblyOperator

    RETURN TranslationError::NO_ERRORS;
}
