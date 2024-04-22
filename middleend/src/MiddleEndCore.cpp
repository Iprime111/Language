#include <cassert>

#include "MiddleEndCore.h"
#include "TreeReader.h"

TranslationError DestroyTranslationContext (TranslationContext *context) {
    assert (context);

    Tree::DestroySubtreeNode (&context->abstractSyntaxTree, context->abstractSyntaxTree.root);

    for (size_t nameIndex = 0; nameIndex < context->nameTable.currentIndex; nameIndex++) {
        if (context->nameTable.data [nameIndex].keyword == Keyword::NOT_KEYWORD) {
            free (const_cast <char *> (context->nameTable.data [nameIndex].name));
        }
    }

    DestroyBuffer (&context->nameTable);

    for (size_t localTableIndex = 0; localTableIndex < context->localTables.currentIndex; localTableIndex++) {
        DestroyBuffer (&context->localTables.data [localTableIndex].items);
    }

    DestroyBuffer (&context->localTables);

    return TranslationError::NO_ERRORS;
}
