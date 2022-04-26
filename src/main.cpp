#include <iostream>
#include <SourceManager.h>
#include <Lexer.h>
#include <Parser.h>
#include <ASTContext.h>
#include <AstPrinter.h>

int main(int argc, char *argv[]) {
    using namespace reflex;

    SourceManager srcManager;
    auto &file = srcManager.open("language_test.reflex");

    ASTContext astContext;
    Lexer lexer(file, file.content(), getTokenDescription(), getKeywordDescription());
    Parser parser(astContext, lexer);
    try {
        auto astRoot = parser.parseCompilationUnit();

        AstPrinter printer(std::cout);
        printer.visit(*astRoot);
    } catch (UnrecoverableError &err) {
        std::cout << err.getErrorLocation()->getLocationString() << std::endl;
        err.getErrorLocation()->printSourceRegion(std::cout, true);
        std::cout << err.getErrorMessage() << std::endl;
    }

    return 0;
}
