#ifndef IR_PRINTER_H_
#define IR_PRINTER_H_

#include "BasicBlock.h"
#include "Buffer.h"
#include "Function.h"
#include "IRContext.h"
#include "MachineOpcodes.h"

namespace IR {
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
    
            void PrintFunction (Function   *function);
            void PrintBlock    (BasicBlock *basicBlock);
            void PrintOpcode   (Opcode     *opcode);
    };
}
#endif
