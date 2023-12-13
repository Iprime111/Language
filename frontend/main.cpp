#include "Buffer.h"
#include "FrontendCore.h"
#include "Lexer.h"

int main () {
    CompilationContext context = {};

    InitCompilationContext (&context);

    LexicalAnalysis (&context, "while aaa { b + 1 }");

    DumpTokenTable (&context);
}
