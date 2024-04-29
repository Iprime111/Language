#ifndef FUNCTION_TYPE_H_
#define FUNCTION_TYPE_H_

#include "Buffer.h"

class IntegerType;
struct TranslationContext;
struct TypesImplementation;

class Type {
    friend struct TypesImplementation;

    public:
        virtual ~Type () = 0;

        size_t GetSize ();

        static IntegerType *GetInt64Ty (TranslationContext *context);
    
    protected:
        Type (size_t size);
    
    private:
        size_t size = 0;
};

class IntegerType final : public Type {
    friend struct TypesImplementation;

    public:
        bool IsUnsigned ();

    private:
        bool isUnsigned = false;

        IntegerType (size_t size, bool isUnsigned);
};

struct FunctionType {
    const Type           *returnValue = {};
    Buffer <const Type *> params      = {}; 
};

struct TypesImplementation {
    IntegerType int64Type = IntegerType (8, false);
};

#endif
