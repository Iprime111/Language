#include "ReverseFrontendCore.h"
#include "Buffer.h"

TranslationError InitTranslationContext (TranslationContext *context) {
    PushLog (2);
}

TranslationError DestroyTranslationContext (TranslationContext *context) {
    PushLog (2);

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

    RETURN TranslationError::NO_ERRORS;
}
