#include <cassert>
#include <cstdio>

#include "IRPrinter.h"
#include "Function.h"
#include "IRContext.h"
#include "Instruction.h"

namespace IR {
    IRPrinter::IRPrinter (IRContext *context, MachineOpcodes *opcodes) : context (context), opcodes (opcodes) {}
    
    const std::string *IRPrinter::GetBuffer  () { return &buffer; }
    
    //TODO print to buffer
    //TODO: change to Constant to process global variables
    size_t IRPrinter::PrintIR (FILE *stream) {
        opcodes->ProcessProgramEnter ();
    
        for (std::list <Function *>::iterator functionIterator = context->functions.begin (); 
             functionIterator != context->functions.end (); functionIterator++)
                PrintFunction (*functionIterator);

        const std::vector <Opcode *> &opcodesVector = opcodes->GetOpcodes ();
        std::string opcodesContent = {};

        for (size_t opcodeIndex = 0; opcodeIndex < opcodesVector.size (); opcodeIndex++)
            opcodesContent += opcodesVector [opcodeIndex]->opcodeContent;

        fprintf (stream, "%s", opcodesContent.c_str ());

        return opcodesContent.size ();
    }
    
    void IRPrinter::PrintFunction (Function *function) {
        assert (function);
    
        opcodes->ProcessFunctionEnter (function);
    
        for (std::list <BasicBlock *>::iterator blockIterator = function->basicBlocks.begin (); 
             blockIterator != function->basicBlocks.end (); blockIterator++)
                PrintBlock (*blockIterator);
    }
    
    void IRPrinter::PrintBlock (BasicBlock *basicBlock) {
        assert (basicBlock);
    
        opcodes->ProcessBlockEnter (basicBlock);
    
        for (std::list <Instruction *>::iterator instructionIterator = basicBlock->instructions.begin ();
             instructionIterator != basicBlock->instructions.end (); instructionIterator++)
                opcodes->GetOpcodeByInstruction (*instructionIterator);
    }
}
