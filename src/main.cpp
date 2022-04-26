#include <iostream>
#include <SourceManager.h>
#include <Lexer.h>
#include <Parser.h>
#include <ASTContext.h>

int main(int argc, char *argv[]) {
    using namespace reflex;

    SourceManager srcManager;
    auto &file = srcManager.open("language_test.reflex");

    ASTContext astContext;
    Lexer lexer(file, file.content(), getTokenDescription(), getKeywordDescription());
    Parser parser(astContext, lexer);

    return 0;
}
