#include <algorithm>
#include <cstddef>
#include <string>

#include "BasicBlock.h"
#include "IRContext.h"
#include "Instruction.h"
#include "User.h"
#include "Function.h"
#include "Value.h"

namespace IR {
    BasicBlock::BasicBlock (std::string &name, Value *blockParent) : 
        User (ValueId::BASIC_BLOCK, nullptr), name (name), blockLength (0) {

        parent = blockParent;
    }

    BasicBlock::~BasicBlock () {
        for (std::list <Instruction *>::iterator instructionsIterator = instructions.begin (); 
                instructionsIterator != instructions.end (); instructionsIterator++) {

            delete *instructionsIterator;
        }
    }
    
    const std::string &BasicBlock::GetName       () const { return name; }
    size_t             BasicBlock::GetLength     () const { return blockLength; }
    
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
    
    BasicBlock *BasicBlock::Create (const char *name, Function *function, IRContext *context) {
        if (!name || !function)
            return nullptr;

        std::string blockName = name;

        size_t blockIndex = context->blockNames [blockName];

        fprintf (stderr, "%s %lu\n", name, blockIndex);

        if (blockIndex > 0)
            blockName += "_" + std::to_string (blockIndex);

        context->blockNames [blockName] = blockIndex + 1;//TODO fix
    
        BasicBlock *newBlock = new BasicBlock (blockName, function);
    
        function->basicBlocks.push_back (newBlock);
    
        return newBlock;
    }
}
