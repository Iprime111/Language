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

        // TODO base class should NOT know about any of implementations
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
        bool isUnsigned = false; // TODO I'd better move it as default-value in ctor

        IntegerType (size_t size, bool isUnsigned);
};

struct FunctionType {
    // TODO I don't like these fields are public
    const Type                *returnValue = {};
    std::vector <const Type *> params      = {}; 
};

struct TypesImplementation {
    // TODO since this field is public, user can reassign it to something different
    // it's better to define namespace-global const int64Type
    IntegerType int64Type = IntegerType (8, false);
};

#endif
