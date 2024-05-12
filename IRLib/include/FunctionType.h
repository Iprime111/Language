#ifndef FUNCTION_TYPE_H_
#define FUNCTION_TYPE_H_

#include <vector>
#include <cstddef>

namespace IR {
    class IntegerType;
    struct IRContext;
    class TypesImplementation;
    
    //TODO split to different files
    
    enum class TypeId {
        NO_TYPE      = 0,
        INTEGER_TYPE = 1,
    };
    
    class Type {
        friend class TypesImplementation;
    
        public:
            virtual ~Type () = default;
    
            size_t GetSize   () const;
            TypeId GetTypeId () const;
    
        protected:
            Type (size_t bitsCount, TypeId typeId);
        
        private:
            size_t bitsCount = 0;
            size_t size      = 0;
            TypeId typeId    = TypeId::NO_TYPE;
    };
    
    class IntegerType final : public Type {
        friend class TypesImplementation;
    
        public:
            ~IntegerType () = default;    
    
            bool IsUnsigned () const;
    
        private:
            bool isUnsigned = false;
    
            IntegerType (size_t bitsCount, bool isUnsigned);
    };
    
    struct FunctionType {
        const Type                *returnValue = {};
        std::vector <const Type *> params      = {}; 
    };
    
    class TypesImplementation {
        public:
             const IntegerType *GetInt64Ty () const;
             const IntegerType *GetInt1Ty  () const;
    
        private:
            IntegerType int64Type = IntegerType (64, false);
            IntegerType int1Type  = IntegerType (1,  false);
    };
    
    const TypesImplementation IR_TYPES_ = {};
}
#endif
