#ifndef FUNCTION_TYPE_H_
#define FUNCTION_TYPE_H_

#include <cstddef>

namespace IR {
    struct IRContext;
    class TypesImplementation;
    
    enum class TypeId {
        NO_TYPE      = 0,
        INTEGER_TYPE = 1,
        FLOAT_TYPE   = 2,
    };
    
    class Type {
        friend class TypesImplementation;
    
        public:
            virtual ~Type () = default;
    
            size_t GetSize   () const;
            TypeId GetTypeId () const;
    
        protected:
            explicit Type (size_t bitsCount, TypeId typeId);
        
        private:
            size_t bitsCount = 0;
            size_t size      = 0;
            TypeId typeId    = TypeId::NO_TYPE;
    };
    
    class IntegerType final : public Type {
        friend class TypesImplementation;
    
        public:
            bool IsUnsigned () const;
    
        private:
            bool isUnsigned = false;
    
            explicit IntegerType (size_t bitsCount, bool isUnsigned);
    };

    class FloatType : public Type {
        friend class TypesImplementation;

        private:
            explicit FloatType (size_t bitsCount);
    };
    
    class TypesImplementation {
        public:
             const IntegerType *GetInt64Ty  () const;
             const IntegerType *GetInt1Ty   () const;
             const FloatType   *GetDoubleTy () const;
    
        private:
            IntegerType int64Type  = IntegerType (64, false);
            IntegerType int1Type   = IntegerType (1,  false);
            FloatType   doubleType = FloatType   (64);
    };
    
    const TypesImplementation IR_TYPES_ = {};
}
#endif
