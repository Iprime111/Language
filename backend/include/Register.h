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

constexpr const Register *GetReg (size_t index) {
    return &AVAILABLE_REGISTERS [index];
}

constexpr const Register* RAX = &AVAILABLE_REGISTERS [0];
constexpr const Register* RBX = &AVAILABLE_REGISTERS [1];
constexpr const Register* RCX = &AVAILABLE_REGISTERS [2];
constexpr const Register* RDX = &AVAILABLE_REGISTERS [3];
constexpr const Register* RSI = &AVAILABLE_REGISTERS [4];
constexpr const Register* RDI = &AVAILABLE_REGISTERS [5];
constexpr const Register* R8  = &AVAILABLE_REGISTERS [6];
constexpr const Register* R9  = &AVAILABLE_REGISTERS [7];
constexpr const Register* R10 = &AVAILABLE_REGISTERS [8];
constexpr const Register* R11 = &AVAILABLE_REGISTERS [9];
constexpr const Register* R12 = &AVAILABLE_REGISTERS [10];
constexpr const Register* R13 = &AVAILABLE_REGISTERS [11];
constexpr const Register* R14 = &AVAILABLE_REGISTERS [12];
constexpr const Register* R15 = &AVAILABLE_REGISTERS [13];

constexpr size_t AVAILABLE_REGISTERS_COUNT = sizeof (AVAILABLE_REGISTERS) / sizeof (Register);

class RegisterSet {
    public:
        RegisterSet           ();
        RegisterSet           (size_t count, ...);
        RegisterSet           (size_t count, Register **registers);

        Register *GetRegister (size_t index);
        size_t    GetCount    ();

    private:
        Register *registers [AVAILABLE_REGISTERS_COUNT] = {};
        size_t count = 0;
};

#endif
