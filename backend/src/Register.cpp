#include <cstdarg>

#include "Register.h"

RegisterSet::RegisterSet () {
    this->count = 0;
}

RegisterSet::RegisterSet (size_t count, ...) {
    va_list args;
    va_start (args, count);

    this->count = count;

    for (size_t registerIndex = 0; registerIndex < count; registerIndex++)
        this->registers [registerIndex] = va_arg (args, Register *);

    va_end (args);
}

Register *RegisterSet::GetRegister (size_t index) {
    if (index >= this->count)
        return nullptr;

    return this->registers [index];
}

size_t RegisterSet::GetCount () {
    return this->count;
}


