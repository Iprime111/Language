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
    //TODO comments in listing
    //TODO: change to Constant to process global variables
    void IRPrinter::PrintIR () {
        PrintOpcode (opcodes->ProcessProgramEnter ());
        printf ("\n");
    
        for (std::list <Function *>::iterator functionIterator = context->functions.begin (); 
             functionIterator != context->functions.end (); functionIterator++)
                PrintFunction (*functionIterator);
    }
    
    void IRPrinter::PrintFunction (Function *function) {
        assert (function);
    
        PrintOpcode (opcodes->ProcessFunctionEnter (function));
    
        for (std::list <BasicBlock *>::iterator blockIterator = function->basicBlocks.begin (); 
             blockIterator != function->basicBlocks.end (); blockIterator++)
                PrintBlock (*blockIterator);
    }
    
    void IRPrinter::PrintBlock (BasicBlock *basicBlock) {
        assert (basicBlock);
    
        PrintOpcode (opcodes->ProcessBlockEnter (basicBlock));
    
        for (std::list <Instruction *>::iterator instructionIterator = basicBlock->instructions.begin ();
             instructionIterator != basicBlock->instructions.end (); instructionIterator++)
                PrintOpcode (opcodes->GetOpcodeByInstruction (*instructionIterator));
    }
    
    void IRPrinter::PrintOpcode (Opcode *opcode) {

        if (opcode)
            printf ("%s", opcode->opcodeContent.c_str ());
    }
}
