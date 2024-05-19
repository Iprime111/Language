#include <cassert>

#include "AST/TranslationContext.h"
#include "Codegen/BoolOperations.h"
#include "ConsoleParser.h"
#include "IRPrinter.h"
#include "Opcodes.h"
#include "TreeReader/TreeReader.h"
#include "Codegen/IntegerOperators.h"

static char *TreeFile     = nullptr;
static char *NamesFile    = nullptr;
static char *TreeDumpFile = nullptr;
static char *AssemblyFile = nullptr;

static char *GetFileContent     (const char *filename);
static void SetTreeFilename     (char **name);
static void SetNamesFilename    (char **name);
static void SetTreeDumpFilename (char **name);
static void SetAssemblyFilename (char **name);

int main (int argc, char **argv) {

    RegisterFlag ("-t", "--tree",  SetTreeFilename,     1);
    RegisterFlag ("-n", "--names", SetNamesFilename,    1);
    RegisterFlag ("-d", "--dump",  SetTreeDumpFilename, 1);
    RegisterFlag ("-S", "-S",      SetAssemblyFilename, 1);

    ParseFlags (argc, argv);

    char *treeData      = GetFileContent (TreeFile);
    char *nameTableData = GetFileContent (NamesFile);

    if (!treeData || !nameTableData) {
        printf ("Can not read data from file\n");
        return 0;
    }

    Ast::TranslationContext context = Ast::TranslationContext ();
    Ast::TreeReader         reader  = Ast::TreeReader (&context);

    reader.ReadNameTables (nameTableData);
    reader.ReadTree       (treeData);

    free (treeData);
    free (nameTableData);

    RegisterIntegerOperations (&context);
    RegisterBoolOperations    (&context);

    //if (TreeDumpFile)
    //    DumpSyntaxTree (&context, TreeDumpFile);
    
    FILE *assemblyStream = fopen (AssemblyFile, "w");

    if (assemblyStream) {
        context.root->Codegen (&context);

        context.builder.SetEntryPoint (context.functions [context.entryPoint]);

        x86Opcodes     opcodes = x86Opcodes (&context.irContext);
        IR::IRPrinter  printer = IR::IRPrinter (&context.irContext, &opcodes);

        printer.PrintIR ();

        fclose (assemblyStream);
    }

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

static void SetTreeDumpFilename (char **name) {
    assert (name);
    assert (*name);

    TreeDumpFile = name [0];
}

static void SetAssemblyFilename (char **name) {
    assert (name);
    assert (*name);

    AssemblyFile = name [0];
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
