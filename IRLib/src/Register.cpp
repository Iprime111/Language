#include <cstdarg>
#include <cassert>

#include "Register.h"

RegisterSet::RegisterSet () : count(0) {}

RegisterSet::RegisterSet (size_t registersCount, ...) {
    va_list args;
    va_start (args, count);

    if (registersCount > AVAILABLE_REGISTERS_COUNT)
        count = 0;

    registersCount = count;

    for (size_t registerIndex = 0; registerIndex < count; registerIndex++)
        registers [registerIndex] = va_arg (args, Register *);

    va_end (args);
}

RegisterSet::RegisterSet (size_t registersCount, Register **registers) {
    if (!registers)
        return;

    if (registersCount > AVAILABLE_REGISTERS_COUNT)
        count = 0;

    count = registersCount;

    for (size_t registerIndex = 0; registerIndex < count; registerIndex++)
        registers [registerIndex] = registers [registerIndex];

}

Register *RegisterSet::GetRegister (size_t index) {
    if (index >= count)
        return nullptr;

    return registers [index];
}

size_t RegisterSet::GetCount () {
    return count;
}


