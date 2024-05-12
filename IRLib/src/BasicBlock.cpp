#include <algorithm>
#include <cstddef>

#include "BasicBlock.h"
#include "IRContext.h"
#include "Instruction.h"
#include "User.h"
#include "Function.h"
#include "Value.h"

namespace IR {
    BasicBlock::BasicBlock (char *name, Value *blockParent, size_t labelIndex) : User (ValueId::BASIC_BLOCK, nullptr), name (name), blockLength (0), 
                                                                                blockIndex (labelIndex) {
        parent = blockParent;
    }
    
    char  *BasicBlock::GetName       () const { return name; }
    size_t BasicBlock::GetLength     () const { return blockLength; }
    size_t BasicBlock::GetLabelIndex () const { return blockIndex; }
    
    Instruction *BasicBlock::InsertTail (Instruction *newInstruction) {
        if (!newInstruction)
            return nullptr;
    
        instructions.push_back (newInstruction);
        
        return newInstruction;
    }
    
    Instruction *BasicBlock::InsertAfterPoint (Instruction *newInstruction, Instruction *insertPoint) {
        if (!newInstruction || !insertPoint)
            return nullptr;
    
        std::list <Instruction *>::iterator iterator = std::find (instructions.begin (), instructions.end (), insertPoint);
    
        if (iterator != instructions.end ()) {
            iterator++;
    
            instructions.insert (iterator, newInstruction);
            return newInstruction;
        }
    
        return nullptr;
    }
    
    BasicBlock *BasicBlock::Create (char *name, Function *function, IRContext *context) {
        if (!name || !function)
            return nullptr;
    
        BasicBlock *newBlock = new BasicBlock (name, function, context->currentLabelIndex++);
    
        function->basicBlocks.push_back (newBlock);
    
        return newBlock;
    }
}
