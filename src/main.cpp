#include <iostream>
#include <SourceManager.h>
#include <Lexer.h>

int main(int argc, char *argv[]) {
    using namespace reflex;

    SourceManager srcManager;
    auto &file = srcManager.open("language_test.reflex");

    Lexer lexer(file, file.content(), getTokenDescription(), getKeywordDescription());
    while (lexer.hasNext()) {
        auto tok = lexer.nextToken();

        if (!tok.isTrivial())
            tok.getLocInfo()->printSourceRegion(std::cout, true);
    }

    //    AstContextManager ctx;
//    Parser parser(&lexer, &ctx);
//
//    auto root = parser.parseCompilationUnit();
//    AstPrinter printer(std::cout);
//    TypeContextManager manager;
//
//    AstRecordTypeAnnotator recordTypeAnnotator{manager};
//    auto table = recordTypeAnnotator.annotate(root);
//    table->resetPrefixCounter();
//
//    AstDeclTypeAnnotator declTypeAnnotator{manager, table};
//    declTypeAnnotator.annotate(root);
//    table->resetPrefixCounter();
//
//    AstExpressionAnnotator expressionAnnotator{manager, table};
//    expressionAnnotator.annotate(root);
//    table->resetPrefixCounter();
//
//    manager.dump(std::cout);
//    std::cout << std::string(75, '=') << std::endl;
//    table->printScope(std::cout, 0);
//    std::cout << std::string(75, '=') << std::endl;
//    printer.visit(root);
//    std::cout << std::string(75, '=') << std::endl;

    return 0;
}
