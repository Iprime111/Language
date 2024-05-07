#include "IRPrinter.h"
#include "Buffer.h"
#include "IRContext.h"
#include "Instruction.h"
#include <cstdio>

IRPrinter::IRPrinter (IRContext *context, MachineOpcodes *opcodes) : context (context), opcodes (opcodes) {
    InitBuffer (&buffer);
}

IRPrinter::~IRPrinter () {
    DestroyBuffer (&buffer);
}

const Buffer <char> *IRPrinter::GetBuffer  () { return &buffer; }

//TODO comments in listing

//TODO: change to Constant to process global variables
void IRPrinter::PrintIR () {

    printf ("Print started\n");
    for (size_t functionIndex = 0; functionIndex < context->functions.size (); functionIndex++) {
        Function *currentFunction = context->functions [functionIndex];

        Opcode *opcode = opcodes->ProcessFunctionEnter (currentFunction);

        //TODO PrintOpcode ()
        if (opcode)
            printf ("%s", opcode->opcodeContent.c_str ());

        PrintFunction (currentFunction);
    }
}

void IRPrinter::PrintFunction (Function *function) {
    for (size_t blockIndex = 0; blockIndex < function->basicBlocks.size (); blockIndex++) {
        Instruction *currentInstruction = function->basicBlocks [blockIndex]->GetHead ();

        while (currentInstruction) {
            Opcode *opcode = opcodes->GetOpcodeByInstruction (currentInstruction);
    
            if (opcode)
                printf ("%s", opcode->opcodeContent.c_str ());

            currentInstruction = currentInstruction->next;
        }
    }
}
