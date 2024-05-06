#ifndef FUNCTION_TYPE_H_
#define FUNCTION_TYPE_H_

#include <vector>

class IntegerType;
struct IRContext;
struct TypesImplementation;

//TODO split to different files

enum class TypeId {
    NO_TYPE      = 0,
    INTEGER_TYPE = 1,
};

class Type {
    friend struct TypesImplementation;

    public:
        virtual ~Type () = default;

        size_t GetSize   () const;
        TypeId GetTypeId () const;

        static const IntegerType *GetInt64Ty (IRContext *context);
    
    protected:
        Type (size_t size, TypeId typeId);
    
    private:
        size_t size   = 0;
        TypeId typeId = TypeId::NO_TYPE;
};

class IntegerType final : public Type {
    friend struct TypesImplementation;

    public:
        ~IntegerType () = default;    

        bool IsUnsigned () const;

    private:
        bool isUnsigned = false;

        IntegerType (size_t size, bool isUnsigned);
};

struct FunctionType {
    const Type                *returnValue = {};
    std::vector <const Type *> params      = {}; 
};

struct TypesImplementation {
    IntegerType int64Type = IntegerType (8, false);
};

#endif
