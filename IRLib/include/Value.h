#ifndef IR_H_
#define IR_H_

struct IRContext;

enum class ValueType {
    VALUE        = 1 << 0,
    INSTRUCTION  = 1 << 1,
    BASIC_BLOCK  = 1 << 2,
    FUNCTION     = 1 << 3,
    CONSTANT     = 1 << 4,
};

class Value {
    public:
        Value *next   = nullptr;
        Value *prev   = nullptr;
        Value *parent = nullptr;

        virtual ~Value () = 0;

        ValueType GetType ();

    protected:
        Value (ValueType valueType);

    private:
        ValueType valueType = ValueType::VALUE;

};

#endif
