#include <cstdarg>
#include <cassert>

#include "Register.h"

RegisterSet::RegisterSet () : count(0) {
    this->count = 0; // TODO never write this->
}

RegisterSet::RegisterSet (size_t count, ...) {
    va_list args;
    va_start (args, count);

    if (count > AVAILABLE_REGISTERS_COUNT)
        this->count = 0;

    this->count = count;

    for (size_t registerIndex = 0; registerIndex < this->count; registerIndex++)
        this->registers [registerIndex] = va_arg (args, Register *);

    va_end (args);
}

RegisterSet::RegisterSet (size_t count, Register **registers) {
    assert (registers);

    if (count > AVAILABLE_REGISTERS_COUNT)
        this->count = 0;

    this->count = count;

    for (size_t registerIndex = 0; registerIndex < this->count; registerIndex++)
        this->registers [registerIndex] = registers [registerIndex];

}

Register *RegisterSet::GetRegister (size_t index) {
    if (index >= this->count)
        return nullptr;

    return this->registers [index];
}

size_t RegisterSet::GetCount () {
    return this->count;
}


