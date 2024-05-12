#ifndef TREE_READER_H_
#define TREE_READER_H_

#include "AST/TranslationContext.h"

namespace Ast {
    class TreeReader final {
        public:
            TreeReader (TranslationContext *context);

            void ReadTree       (FILE *stream);
            void ReadNameTables (char *fileContent);

        private:
            TranslationContext *context;

            void ReadGlobalTable (char **fileContent);
            void ReadLocalTable  (char **fileContent, size_t tableIndex);
    };
}

#endif
