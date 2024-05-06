#include "IRPrinter.h"
#include "Buffer.h"
#include "IRContext.h"
#include "Instruction.h"

IRPrinter::IRPrinter (IRContext *context, MachineOpcodes *opcodes) : context (context), opcodes (opcodes) {}

const Buffer <char> *IRPrinter::GetListingBuffer () { return &listingBuffer; }
const Buffer <char> *IRPrinter::GetBinaryBuffer  () { return &listingBuffer; }

//TODO comments in listing

//TODO: change to Constant to process global variables
void IRPrinter::PrintIR () {

    printf ("Print started\n");
    for (size_t functionIndex = 0; functionIndex < context->functions.size (); functionIndex++) {
        PrintFunction (context->functions [functionIndex]);
    }
}

void IRPrinter::PrintFunction (Function *function) {
    for (size_t blockIndex = 0; blockIndex < function->basicBlocks.size (); blockIndex++) {
        Instruction *currentInstruction = function->basicBlocks [blockIndex].GetHead ();

        while (currentInstruction) {
            Opcode *opcode = opcodes->GetOpcodeByInstruction (currentInstruction);
    
            if (opcode)
                printf ("%s", opcode->opcode);

            currentInstruction = currentInstruction->next;
        }
    }
}
