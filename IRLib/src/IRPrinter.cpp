#include <cassert>
#include <cstdio>

#include "IRPrinter.h"
#include "Buffer.h"
#include "Function.h"
#include "IRContext.h"
#include "Instruction.h"

namespace IR {
    IRPrinter::IRPrinter (IRContext *context, MachineOpcodes *opcodes) : context (context), opcodes (opcodes) {
        InitBuffer (&buffer);
    }
    
    IRPrinter::~IRPrinter () {
        DestroyBuffer (&buffer);
    }
    
    const Buffer <char> *IRPrinter::GetBuffer  () { return &buffer; }
    
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
    
        printf ("\n");
    }
    
    void IRPrinter::PrintBlock (BasicBlock *basicBlock) {
        assert (basicBlock);
    
        PrintOpcode (opcodes->ProcessBlockEnter (basicBlock));
    
        for (std::list <Instruction *>::iterator instructionIterator = basicBlock->instructions.begin ();
             instructionIterator != basicBlock->instructions.end (); instructionIterator++)
                PrintOpcode (opcodes->GetOpcodeByInstruction (*instructionIterator));
    
        printf ("\n");
    }
    
    void IRPrinter::PrintOpcode (Opcode *opcode) {
        if (opcode)
            printf ("%s", opcode->opcodeContent.c_str ());
    }
}
