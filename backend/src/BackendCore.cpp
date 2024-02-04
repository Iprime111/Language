#include "BackendCore.h"
#include "Buffer.h"
#include "CustomAssert.h"
#include "NameTable.h"
#include "TreeDefinitions.h"

TranslationError InitTranslationContext (TranslationContext *context) {
    PushLog (3);

    custom_assert (context, pointer_is_null, TranslationError::CONTEXT_ERROR);

    if (InitNameTable (&context->nameTable, false) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN TranslationError::NAME_TABLE_ERROR;
    }

    if (InitBuffer (&context->localTables) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN TranslationError::NAME_TABLE_ERROR;
    }

    AddLocalNameTable (0, &context->localTables);

    context->error      = TranslationError::NO_ERRORS;

    RETURN TranslationError::NO_ERRORS;
}

TranslationError DestroyTranslationContext (TranslationContext *context) {
    PushLog (3);

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
