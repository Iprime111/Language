#ifndef IR_BUILDER_H_
#define IR_BUILDER_H_

#include "BackendCore.h"
#include "Ir.h"

struct IRBuilder {
    TranslationContext *context     = nullptr;
    BasicBlock         *insertPoint = {};
};

#endif 
