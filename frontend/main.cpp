#include "Buffer.h"
#include "FrontendCore.h"
#include "Lexer.h"
#include "Parser.h"
#include "Dump.h"

int main () {
    CompilationContext context = {};

    InitCompilationContext (&context);

    LexicalAnalysis (&context, "Сап конфешнс\n дедекиндово сечение множества действительных чисел foo = 5 .");

    DumpTokenTable (&context);
    
    ParseCode (&context);

    DumpSyntaxTree (&context, "tree_dump.dot");

    DestroyCompilationContext (&context);

    return 0;
}
