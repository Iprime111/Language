#include "MiddleEndCore.h"
#include "Optimizations.h"
#include "Differentiator.h"
#include "TreeReader.h"
#include "TreeSaver.h"

static char *GetFileContent (const char *filename);

int main (int argc, char **argv) {
    PushLog (1);

     if (argc < 3) {
        printf ("Console arguments (syntax tree file and name table file paths) expected\n");
        return 0;
    }

    char *treeData      = GetFileContent (argv [1]);
    char *nameTableData = GetFileContent (argv [2]);

    if (!treeData || !nameTableData) {
        printf ("Can not read data from file\n");
        return 0;
    }

    TranslationContext context = {};

    ReadSyntaxTree (&context.abstractSyntaxTree, &context.entryPoint, treeData);
    ReadNameTables (&context.nameTable, &context.localTables, nameTableData);

    free (treeData);
    free (nameTableData);
    
    DifferentiationTraversal (&context, context.abstractSyntaxTree.root);
    DoOptimizations          (&context);

    FILE *treeFile = fopen (argv [1], "w");

    if (!treeFile) {
        printf ("Can not open tree file for writing\n");
        RETURN 0;
    }

    SaveTree (&context, treeFile);
    fclose   (treeFile);

    DestroyTranslationContext (&context);
}

static char *GetFileContent (const char *filename) {
    PushLog (4);

    FILE *sourceFile = fopen (filename, "r");

    if (!sourceFile) {
        printf ("Can not open source file\n");
        return NULL;
    }
    
    fseek (sourceFile, 0, SEEK_END);
    size_t fileSize = (size_t) ftell (sourceFile);
    fseek (sourceFile, 0, SEEK_SET);

    char *sourceData = (char *) calloc (fileSize + 1, sizeof (char));
    fread (sourceData, fileSize, 1, sourceFile);

    sourceData [fileSize] = '\0';
    
    fclose (sourceFile);

    RETURN sourceData;
}
