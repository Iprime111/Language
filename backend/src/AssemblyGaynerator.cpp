#include <cstdio>

#include "AssemblyGaynerator.h"
#include "BackendCore.h"
#include "Buffer.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeReader.h"

#define WriteString(data)                                                                       \
    do {                                                                                        \
        if (WriteStringToBuffer (outputBuffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {    \
            return TranslationError::OUTPUT_FILE_ERROR;                                         \
        }                                                                                       \
    } while (0)

static TranslationError LoadProgram               (TranslationContext *context, Buffer <char> *outputBuffer);

static TranslationError TreeTraversal             (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);

static TranslationError WriteConstant             (double constant, Buffer <char> *outputBuffer);
static TranslationError WriteIdentifier           (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError NewFunction               (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError NewVariable               (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError WriteFunctionCall         (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError WriteKeyword              (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);
static TranslationError WriteIdentifierMemoryCell (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex);

static TranslationError WriteCreationSource       (TranslationContext *context, Buffer <char> *outputBuffer, CreationData creationData);

static TranslationError WriteLabel                (TranslationContext *context, Buffer <char> *outputBuffer, char *labelName, size_t labelIndex);

TranslationError GenerateAssembly (TranslationContext *context, FILE *outputStream) {
    PushLog (1);

    custom_assert (context,                          pointer_is_null, TranslationError::CONTEXT_ERROR);
    custom_assert (context->abstractSyntaxTree.root, pointer_is_null, TranslationError::TREE_ERROR);
    custom_assert (outputStream,                     pointer_is_null, TranslationError::OUTPUT_FILE_ERROR);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        return TranslationError::OUTPUT_FILE_ERROR;
    }

    LoadProgram (context, &outputBuffer);
    TreeTraversal (context, context->abstractSyntaxTree.root, &outputBuffer, 0);

    fwrite (outputBuffer.data, outputBuffer.currentIndex, 1, outputStream);

    DestroyBuffer (&outputBuffer);

    return TranslationError::NO_ERRORS;
}

static TranslationError LoadProgram (TranslationContext *context, Buffer <char> *outputBuffer) {
    PushLog (4);

    char initialAddressBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (initialAddressBuffer, MAX_NUMBER_LENGTH, "%d", INITIAL_ADDRESS);

    Source      (0, "SETTING RBP", "PROGRAM LOADER");
    WriteString ("\tpush ");
    WriteString (initialAddressBuffer);
    WriteString ("\n\tpop rbp\n");

    Source      (0, "ENTRY POINT CALL", "PROGRAMM LOADER");
    WriteString ("\tcall ");
    WriteString (context->nameTable.data [context->entryPoint].name);
    WriteString ("\n\thlt");

    return TranslationError::NO_ERRORS;
}

static TranslationError TreeTraversal (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (2);

    if (!node) {
        return TranslationError::NO_ERRORS;
    }

    #define NextCall(enumMember, function)                                  \
        case NodeType::enumMember: {                                        \
            function (context, node, outputBuffer, currentNameTableIndex);  \
            break;                                                          \
        }

    switch (node->nodeData.type) {

        case NodeType::TERMINATOR: {
            return TranslationError::TREE_ERROR;
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

        NextCall (STRING,                 WriteIdentifier);
        NextCall (KEYWORD,              WriteKeyword);
        NextCall (FUNCTION_DEFINITION,  NewFunction);
        NextCall (VARIABLE_DECLARATION, NewVariable);
        NextCall (FUNCTION_CALL,        WriteFunctionCall);
    }

    #undef NextCall

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteConstant (double constant, Buffer <char> *outputBuffer) {
    PushLog (4);
    
    char numberBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lg", constant);

    WriteString ("\tpush ");
    WriteString (numberBuffer);
    WriteString ("\n");

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteIdentifier (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (4);

    WriteString ("\tpush ");

    WriteIdentifierMemoryCell (context, node, outputBuffer, currentNameTableIndex);

    return TranslationError::NO_ERRORS;
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
        
            return TranslationError::NO_ERRORS;
        }
    }

    identifierIndex = GetIndexInLocalTable (0, &context->localTables, node->nodeData.content.nameTableIndex, LocalNameType::VARIABLE_IDENTIFIER);

    if (identifierIndex < 0) {
        return TranslationError::TREE_ERROR;
    }

    identifierIndex += INITIAL_ADDRESS;
    snprintf (indexBuffer, MAX_NUMBER_LENGTH, "%d", identifierIndex);

    WriteString ("[");
    WriteString (indexBuffer);
    WriteString ("]\n");

    return TranslationError::NO_ERRORS;

}

static TranslationError NewFunction (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (4);

    int tableIndex = GetLocalNameTableIndex ((int) node->nodeData.content.nameTableIndex, &context->localTables);

    Source ((size_t) tableIndex, "FUNCTION LABEL", "FUNCTION DECLARATION");
    WriteString (context->nameTable.data [node->nodeData.content.nameTableIndex].name);
    WriteString (":\n");

    if (tableIndex < 0) {
        return TranslationError::NAME_TABLE_ERROR;
    }

    if (node->right) {
        TreeTraversal (context, node->right, outputBuffer, tableIndex);
    } else {
        return TranslationError::TREE_ERROR;
    }

    return TranslationError::NO_ERRORS;
}

static TranslationError NewVariable (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (4);

    //TODO: variable size
    context->localTables.data [currentNameTableIndex].tableSize += 1;

    if (node->right && node->right->nodeData.type == NodeType::KEYWORD) {
        TreeTraversal (context, node->right, outputBuffer, currentNameTableIndex);
    }

    return TranslationError::NO_ERRORS; 
}

static TranslationError WriteFunctionCall (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, int currentNameTableIndex) {
    PushLog (4);

    size_t blockIndex = ++context->operatorsCount.callCount;
    char  *blockName  = "CALL OPERATOR";

    if (!node->right) {
        return TranslationError::TREE_ERROR;
    }

    context->areCallArguments = true;

    Source      (blockIndex, "STACK FRAME SAVE", blockName);
    WriteString ("\tpush rbp\n");

    Source      (blockIndex, "CALL ARGUMENTS", blockName);

    if (node->left) {
        TreeTraversal (context, node->left, outputBuffer, currentNameTableIndex);
    }

    char stackFrameSizeBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (stackFrameSizeBuffer, MAX_NUMBER_LENGTH, "%lu", context->localTables.data [currentNameTableIndex].tableSize);
    
    Source      (blockIndex, "STACK FRAME CHANGE", blockName);
    WriteString ("\tpush rbp+");
    WriteString (stackFrameSizeBuffer);
    WriteString ("\n\tpop rbp\n");
    
    Source      (blockIndex, "FUNCTION CALL", blockName);
    WriteString ("\tcall ");
    WriteString (context->nameTable.data [node->right->nodeData.content.nameTableIndex].name);
    WriteString ("\n\tpop rbp\n");

    if (node->parent && !(node->parent->nodeData.type == NodeType::KEYWORD && node->parent->nodeData.content.keyword == Keyword::OPERATOR_SEPARATOR)) { 
        WriteString ("\tpush rax\n");
    }

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteLabel (TranslationContext *context, Buffer <char> *outputBuffer, char *labelName, size_t labelIndex) {
    PushLog (4);
    
    char labelIndexBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (labelIndexBuffer, MAX_NUMBER_LENGTH, "%lu", labelIndex);

    WriteString (labelName);
    WriteString ("_");
    WriteString (labelIndexBuffer);

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteCreationSource (TranslationContext *context, Buffer <char> *outputBuffer, CreationData crationData) {
    PushLog (4);

    char numberBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", crationData.blockIndex);

    WriteString ("\n; CODE BLOCK: ");
    WriteString (crationData.blockName);
    WriteString (", SOURCE: ");
    WriteString (crationData.blockSource);
    WriteString (" (#");
    WriteString (numberBuffer);
    WriteString (")\n");


    return TranslationError::NO_ERRORS;
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
            size_t blockIndex = ++context->operatorsCount.ifCount;
            char  *blockName   = "IF STATEMENT";

            Source (blockIndex, "IF CONDITION", blockName);
            Traversal (left);
            WriteString ("\tpush 0\n\tje ");

            Label ("IF_END", blockIndex);
            WriteString ("\n");
            Source (blockIndex, "IF BODY", blockName);
            Traversal (right);
            NewLabel ("IF_END", blockIndex);
        })

        AssemblyOperator (WHILE, {
            size_t blockIndex = ++context->operatorsCount.whileCount;
            char  *blockName  = "WHILE CYCLE"; 

            Source (blockIndex, "WHILE CONDITION", blockName);
            NewLabel ("WHILE_BEGIN", blockIndex);
            Traversal (left);
            WriteString ("\tpush 0\n\tje ");
            Label ("WHILE_END", blockIndex);
            WriteString ("\n");

            Source (blockIndex, "WHILE BODY", blockName);
            Traversal (right);

            Source (blockIndex, "WHILE END JUMP", blockName);
            WriteString ("\tjmp ");
            Label ("WHILE_BEGIN", blockIndex);
            WriteString ("\n");
            NewLabel ("WHILE_END", blockIndex);
        })

        AssemblyOperator (ASSIGNMENT, {
            size_t blockIndex = ++context->operatorsCount.assignmentCount;
            char  *blockName  = "ASSIGNMENT OPERATOR";

            Source (blockIndex, "ASSIGNMENT EXPRESSION", blockName);
            Traversal (left);
            Source (blockIndex, "ASSIGNMENT CELL", blockName);
            WriteString ("\tpop ");
            MemoryCell (right);
        })

        AssemblyOperator (SIN,                UnaryOperation  ("sin"))
        AssemblyOperator (COS,                UnaryOperation  ("cos"))
        AssemblyOperator (FLOOR,              UnaryOperation  ("floor"))
        AssemblyOperator (SQRT,               UnaryOperation  ("sqrt"))
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
        AssemblyOperator (BREAK_OPERATOR,     WriteString ("\tjmp "); Label ("WHILE_END", context->operatorsCount.whileCount);   WriteString ("\n");) //FIXME: inner breaks
        AssemblyOperator (CONTINUE_OPERATOR,  WriteString ("\tjmp "); Label ("WHILE_END", context->operatorsCount.whileCount); WriteString ("\n");)
        AssemblyOperator (IN,                 WriteString ("\tin\n");)
        AssemblyOperator (OUT,                Traversal (right); WriteString ("\tout\n");)
        AssemblyOperator (OPERATOR_SEPARATOR, BothWayTraversal ();)
        AssemblyOperator (ARGUMENT_SEPARATOR, {
            if (context->areCallArguments) {
                BothWayTraversal ();
            } else {
                Traversal (right); 
                Traversal (left); 
                if (node->left) { 
                    WriteString ("\tpop "); 
                    MemoryCell  (left);
                }
            }
        })

        default: {
            return TranslationError::TREE_ERROR;
        }

    }

    #undef AssemblyOperator

    return TranslationError::NO_ERRORS;
}
