#ifndef USE_H_
#define USE_H_

class User;
class Value;

class Use final {
    public:
        Use ();
        Use (Value *operand);

        Value *GetOperand () const;

    private:
        Value *operand = nullptr;
};

#endif
