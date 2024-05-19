#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "Type.h"
#include "User.h"
#include "InstructionId.h"

namespace IR {
    class Instruction : public User {
        public:
            InstructionId GetInstructionId () const;
    
        protected:
            explicit Instruction (InstructionId instructionId, const Type *instructionType);
            explicit Instruction (InstructionId instructionId, const Type *instructionType, size_t operandsCount);
        
        private:
            InstructionId instructionId;
    };
    
    class StateChanger final : public Instruction {
        public:
            explicit StateChanger (StateChangerId id);
    
            StateChangerId GetStateChangerId () const;
    
        private:
            StateChangerId id;
    };
    
    class UnaryOperator final : public Instruction {
        public:
            explicit UnaryOperator (UnaryOperatorId id, Value *operand);
    
            UnaryOperatorId GetUnaryOperatorId () const;
    
        private:
            UnaryOperatorId id;
    };
    
    class BinaryOperator final : public Instruction {
        public:
            explicit BinaryOperator (BinaryOperatorId id, Value *firstOperand, Value *secondOperand);
    
            BinaryOperatorId GetBinaryOperatorId () const;
    
        private:
            BinaryOperatorId id;
    };
    
    class ReturnOperator final : public Instruction {
        public:
            explicit ReturnOperator (Value *operand);
    };
    
    class CmpOperator final : public Instruction {
        public:
            explicit CmpOperator (CmpOperatorId id, Value *firstOperand, Value *secondOperand);
    
            CmpOperatorId GetCmpOperatorId () const;
    
        private:
            CmpOperatorId id;
    };
    
    class AllocaInstruction final : public Instruction {
        public:
            explicit AllocaInstruction (const Type *type, size_t stackAddress);
    
            size_t GetStackAddress () const;
    
        private:
            size_t stackAddress = 0;
    };
    
    class StoreInstruction final : public Instruction {
        public:
            explicit StoreInstruction (Value *variable, Value *operand);//TODO use pointer type?
    };
    
    class LoadInstruction final : public Instruction {
        public:
            explicit LoadInstruction (Value *variable);
    };
    
    class BasicBlock;
    
    class BranchInstruction final : public Instruction {
        public:
            explicit BranchInstruction (BasicBlock *nextBlock);
            explicit BranchInstruction (Value *condition, BasicBlock *ifTrue, BasicBlock *ifFalse);
    
            void SetTrueBlock  (BasicBlock *block);
            void SetFalseBlock (BasicBlock *block);

            bool IsConditional () const;
    
        private:
            bool isConditional;
    };
    
    class CastInstruction : public Instruction {
        public:
            virtual ~CastInstruction () = default;
    
        protected:
            explicit CastInstruction (Value *castValue, const Type *targetType, CastId castId);
    
        private:
            CastId id;
    };
    
    class Function;

    class CallInstruction final : public Instruction {
        public:
            explicit CallInstruction (Function *calleeFunction, std::vector <Value *> *arguments);
    };

    class InInstruction final : public Instruction {
        public:
            explicit InInstruction ();
    };

    class OutInstruction final : public Instruction {
        public:
            explicit OutInstruction (Value *outValue);
    };
}
#endif
