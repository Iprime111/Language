#ifndef REGISTER_H_
#define REGISTER_H_

#include <cstddef>

struct Register {
    char   *name = nullptr;
    size_t index = 0;
};

constexpr Register AVAILABLE_REGISTERS [] = {
    {"rax", 0},  {"rbx", 1},  {"rcx", 2},  {"rdx", 3},
    {"rsi", 4},  {"rdi", 5},  {"r8",  6},  {"r9",  7},
    {"r10", 8},  {"r11", 9},  {"r12", 10}, {"r13", 11},
    {"r14", 12}, {"r15", 13},
};
#define RAX &AVAILABLE_REGISTERS [0]
#define RBX &AVAILABLE_REGISTERS [1]
#define RCX &AVAILABLE_REGISTERS [2]
#define RDX &AVAILABLE_REGISTERS [3]
#define RSI &AVAILABLE_REGISTERS [4]
#define RDI &AVAILABLE_REGISTERS [5]
#define R8  &AVAILABLE_REGISTERS [6]
#define R9  &AVAILABLE_REGISTERS [7]
#define R10 &AVAILABLE_REGISTERS [8]
#define R11 &AVAILABLE_REGISTERS [9]
#define R12 &AVAILABLE_REGISTERS [10]
#define R13 &AVAILABLE_REGISTERS [11]
#define R14 &AVAILABLE_REGISTERS [12]
#define R15 &AVAILABLE_REGISTERS [13]

constexpr size_t AVAILABLE_REGISTERS_COUNT = sizeof (AVAILABLE_REGISTERS) / sizeof (Register);

class RegisterSet {
    public:
        RegisterSet           ();
        RegisterSet           (size_t count, ...);

        Register *GetRegister (size_t index);
        size_t    GetCount    ();

    private:
        Register *registers [AVAILABLE_REGISTERS_COUNT] = {};
        size_t count = 0;
};

#endif
