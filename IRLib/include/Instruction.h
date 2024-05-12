#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "FunctionType.h"
#include "User.h"
#include "InstructionId.h"

namespace IR {
    class Instruction : public User {
        public:
            InstructionId GetInstructionId () const;
    
            Instruction *next = nullptr;
            Instruction *prev = nullptr;
    
        protected:
            Instruction (InstructionId instructionId, const Type *instructionType);
            Instruction (InstructionId instructionId, const Type *instructionType, size_t operandsCount);
        
        private:
            InstructionId instructionId;
    };
    
    class StateChanger final : public Instruction {
        public:
            StateChanger (StateChangerId id);
    
            StateChangerId GetStateChangerId () const;
    
        private:
            StateChangerId id;
    };
    
    class UnaryOperator final : public Instruction {
        public:
            UnaryOperator (UnaryOperatorId id, Value *operand);
    
            UnaryOperatorId GetUnaryOperatorId () const;
    
        private:
            UnaryOperatorId id;
    };
    
    class BinaryOperator final : public Instruction {
        public:
            BinaryOperator (BinaryOperatorId id, Value *firstOperand, Value *secondOperand);
    
            BinaryOperatorId GetBinaryOperatorId () const;
    
        private:
            BinaryOperatorId id;
    };
    
    class ReturnOperator final : public Instruction {
        public:
            ReturnOperator (Value *operand);
    };
    
    class CmpOperator final : public Instruction {
        public:
            CmpOperator (CmpOperatorId id, Value *firstOperand, Value *secondOperand);
    
            CmpOperatorId GetCmpOperatorId () const;
    
        private:
            CmpOperatorId id;
    };
    
    class AllocaInstruction final : public Instruction {
        public:
            AllocaInstruction (const Type *type, size_t stackAddress);
    
            size_t GetStackAddress () const;
    
        private:
            size_t stackAddress = 0;
    };
    
    class StoreInstruction final : public Instruction {
        public:
            StoreInstruction (AllocaInstruction *variable, Value *operand);//TODO use pointer type?
    };
    
    class LoadInstruction final : public Instruction {
        public:
            LoadInstruction (AllocaInstruction *variable);
    };
    
    class BasicBlock;
    
    class BranchInstruction final : public Instruction {
        public:
            BranchInstruction (BasicBlock *nextBlock);
            BranchInstruction (Value *condition, BasicBlock *ifTrue, BasicBlock *ifFalse);
    
            bool IsConditional ();
    
        private:
            bool isConditional;
    };
    
    class CastInstruction : public Instruction {
        public:
            virtual ~CastInstruction () = default;
    
        protected:
            CastInstruction (Value *castValue, const Type *targetType, CastId castId);
    
        private:
            CastId id;
    };
    
    class TruncCast final : public CastInstruction {
        public:
            TruncCast (Value *castValue, const IntegerType *targetType);
    };

    class Function;

    class CallInstruction final : public Instruction {
        public:
            CallInstruction (Function *calleeFunction, std::vector <Value *> *arguments);
    };
}
#endif
