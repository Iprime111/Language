#ifndef IR_PRINTER_H_
#define IR_PRINTER_H_

#include "BasicBlock.h"
#include "Function.h"
#include "IRContext.h"
#include "MachineOpcodes.h"

namespace IR {
    class IRPrinter final {
        public:
            explicit IRPrinter (IRContext *context, MachineOpcodes *opcodes);
    
            size_t PrintIR (FILE *stream);
    
            const std::string *GetBuffer  ();
        
        private:
            IRContext      *context = nullptr;
            MachineOpcodes *opcodes = nullptr;
    
            std::string buffer = {};
    
            void PrintFunction (Function   *function);
            void PrintBlock    (BasicBlock *basicBlock);
    };
}
#endif
