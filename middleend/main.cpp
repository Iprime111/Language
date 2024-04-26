#include <cassert>

#include "ConsoleParser.h"
#include "MiddleEndCore.h"
#include "Optimizations.h"
#include "Differentiator.h"
#include "TreeReader.h"
#include "TreeSaver.h"

static char *TreeFile   = nullptr;
static char *NamesFile  = nullptr;

static void SetTreeFilename  (char **name);
static void SetNamesFilename (char **name);
static char *GetFileContent  (const char *filename);

int main (int argc, char **argv) {

    RegisterFlag ("-t", "--tree",   SetTreeFilename,   1);
    RegisterFlag ("-n", "--names",  SetNamesFilename,  1);
    
    ParseFlags (argc, argv);

    char *treeData      = GetFileContent (TreeFile);
    char *nameTableData = GetFileContent (NamesFile);

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

    FILE *treeFile = fopen (TreeFile, "w");

    if (!treeFile) {
        printf ("Can not open tree file for writing\n");
        return 0;
    }

    SaveTree (&context, treeFile);
    fclose   (treeFile);

    DestroyTranslationContext (&context);
}

static void SetTreeFilename (char **name) {
    assert (name);
    assert (*name);

    TreeFile = name [0];
}

static void SetNamesFilename (char **name) {
    assert (name);
    assert (*name);

    NamesFile = name [0];
}


static char *GetFileContent (const char *filename) {
    assert (filename);

    FILE *sourceFile = fopen (filename, "r");

    if (!sourceFile) {
        printf ("Can not open source file\n");
        return nullptr;
    }
    
    fseek (sourceFile, 0, SEEK_END);
    size_t fileSize = (size_t) ftell (sourceFile);
    fseek (sourceFile, 0, SEEK_SET);

    char *sourceData = (char *) calloc (fileSize + 1, sizeof (char));
    fread (sourceData, fileSize, 1, sourceFile);

    sourceData [fileSize] = '\0';
    
    fclose (sourceFile);

    return sourceData;
}
