#include <iostream>
#include <fstream>
#include <SourceManager.h>
#include <Lexer.h>
#include <AstContextManager.h>
#include <Parser.h>
#include <AstPrinter.h>
#include <AstTypeAnnotator.h>

int main(int argc, char *argv[]) {
    using namespace reflex;

    std::ifstream file("language_test.reflex");
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    Source src("language_test.reflex", std::move(content));
    Lexer lexer(&src, src.getContent(), getTokenDescription(), getKeywordDescription());
    AstContextManager ctx;
    Parser parser(&lexer, &ctx);

    auto root = parser.parseCompilationUnit();
    AstPrinter printer(std::cout);
    printer.visit(root);

    std::cout << std::endl << "Remaining Tokens: " << std::endl;
    while (lexer.hasNext()) {
        auto tok = lexer.nextToken();
        std::cout << tok << std::endl;
    }

    std::cout << std::endl << "Dumped Class Decls: " << std::endl;

    TypeContextManager manager;
    AstTypeAnnotator annotator(&ctx, &manager);
    annotator.annotate();

    manager.dump(std::cout);

    return 0;
}
