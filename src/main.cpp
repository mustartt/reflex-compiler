#include <iostream>
#include <fstream>
#include <SourceManager.h>
#include <Lexer.h>
#include <AstContextManager.h>
#include <Parser.h>
#include <AstPrinter.h>
#include <TypeContextManager.h>
#include <AstRecordTypeAnnotator.h>
#include <AstDeclTypeAnnotator.h>
#include <ScopeSymbolTypeTable.h>

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
    TypeContextManager manager;

    AstRecordTypeAnnotator recordTypeAnnotator{manager};
    auto table = recordTypeAnnotator.annotate(root);
    AstDeclTypeAnnotator declTypeAnnotator{manager, table};
    declTypeAnnotator.annotate(root);

    printer.visit(root);
    std::cout << std::string(75, '=') << std::endl;
    manager.dump(std::cout);
    std::cout << std::string(75, '=') << std::endl;
    table->printScope(std::cout, 0);

    return 0;
}
