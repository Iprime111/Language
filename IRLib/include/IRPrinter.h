#ifndef IR_PRINTER_H_
#define IR_PRINTER_H_

#include "Buffer.h"
#include "Function.h"
#include "IRContext.h"
#include "MachineOpcodes.h"

class IRPrinter final {
    public:
        IRPrinter  (IRContext *context, MachineOpcodes *opcodes);
        ~IRPrinter ();

        void PrintIR ();

        const Buffer <char> *GetBuffer  ();
    
    private:
        IRContext      *context = nullptr;
        MachineOpcodes *opcodes = nullptr;

        Buffer <char> buffer = {};

        void PrintFunction (Function *function);
};

#endif
