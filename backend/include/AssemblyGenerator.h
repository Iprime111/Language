#ifndef ASSEMBLY_GENERATOR_H_
#define ASSEMBLY_GENERATOR_H_

#include "BackendCore.h"
#include "IRBuilder.h"

#include <stddef.h>

const size_t MAX_NUMBER_LENGTH = 32;
const int    INITIAL_ADDRESS   = 30000;

struct CreationData {
    size_t blockIndex  = 0;
    char  *blockName   = nullptr;
    char  *blockSource = nullptr;
};

#define Traversal(direction)         TreeTraversal (builder, context, node->direction, currentNameTableIndex)

#define UnaryOperation(operation)    builder->CreateUnaryOperator    (UnaryOperatorId::operation, Traversal (right));
#define BinaryOperation(operation)   builder->CreateBinaryOperator   (BinaryOperatorId::operation, Traversal (left), Traversal (right));
#define StateChange(operation)       builder->CreateStateChanger     (StateChangerId::operation);
#define ReturnOperation(returnValue) builder->CreateReturnOperator   (Traversal (right));
#define CmpOperation(operation)      builder->CreateCmpOperator      (CmpOperatorId::operation, Traversal (left), Traversal (right));
#define Load(variable)               builder->CreateLoadInstruction  (variable)
#define Store(variable, expression)  builder->CreateStoreInstruction (variable, expression)

TranslationError GenerateAssembly (IRBuilder *builder, TranslationContext *context);

#endif
