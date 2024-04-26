#include <stdio.h>
#include <cassert>

#include "ConsoleParser.h"
#include "FrontendCore.h"
#include "Lexer.h"
#include "Parser.h"
#include "Dump.h"
#include "ErrorWriter.h"
#include "TreeSaver.h"

static char *SourceFile   = nullptr;
static char *TreeFile     = nullptr;
static char *NamesFile    = nullptr;
static char *TreeDumpFile = nullptr;

static char *GetSourceFileContent (const char *filename);
static void SetSourceFilename     (char **name);
static void SetTreeFilename       (char **name);
static void SetNamesFilename      (char **name);
static void SetTreeDumpFilename   (char **name);

int main (int argc, char **argv) {

    RegisterFlag ("-s", "--source", SetSourceFilename,   1);
    RegisterFlag ("-t", "--tree",   SetTreeFilename,     1);
    RegisterFlag ("-n", "--names",  SetNamesFilename,    1);
    RegisterFlag ("-d", "--dump",   SetTreeDumpFilename, 1);

    
    ParseFlags (argc, argv);
        
    CompilationContext context = {};

    char *sourceData = GetSourceFileContent (SourceFile);

    if (!sourceData) {
        printf ("Can not read source data");
        return 0;
    }

    printf ("Reading code...\n");

    InitCompilationContext (&context, sourceData);

    LexicalAnalysis (&context);

    printf ("Parsing tokens...\n");

    ParseCode (&context);

    printf ("Generating report...\n");

    GenerateErrorHtml (&context, "CompilationReport.html");

    FILE *nameTableFile = fopen (NamesFile, "w");
    FILE *treeFile      = fopen (TreeFile,  "w");

    if (!nameTableFile || !treeFile) {
        printf ("Error while opening output file\n");
        DestroyCompilationContext (&context);
        return 0;
    }

    SaveTree          (&context, treeFile);
    SaveNameTables    (&context, nameTableFile);

    fclose (nameTableFile);
    fclose (treeFile);

    if (TreeDumpFile)
        DumpSyntaxTree (&context, TreeDumpFile);

    DestroyCompilationContext (&context);

    return 0;
}

static void SetSourceFilename (char **name) {
    assert (name);
    assert (*name);

    SourceFile = name [0];
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

static char *GetSourceFileContent (const char *filename) {
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
