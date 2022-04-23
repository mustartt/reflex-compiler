#include <iostream>
#include <fstream>
#include <Source.h>
#include <Lexer.h>
#include <AstContextManager.h>
#include <Parser.h>
#include <AstPrinter.h>
#include <TypeContextManager.h>
#include <AstRecordTypeAnnotator.h>
#include <AstDeclTypeAnnotator.h>
#include <ScopeSymbolTypeTable.h>
#include <AstExpressionAnnotator.h>
#include <CodeGenerator.h>
#include <SourceManager.h>

int main(int argc, char *argv[]) {
    using namespace reflex;

    SourceManager srcManager;
    auto &file = srcManager.open("language_test.reflex");
    auto &loc1 = SourceManager::createSourceLocation(file, 10, 1, 10, 13);
    auto &loc2 = SourceManager::createSourceLocation(file, 13, 1, 13, 1);
    auto &loc3 = SourceManager::mergeSourceLocation(loc2, loc1);

    loc1.printSourceRegion(std::cout, true);
    loc2.printSourceRegion(std::cout, true);
    std::cout << std::endl;
    loc3.printSourceRegion(std::cout, true);

//    std::ifstream file("language_test.reflex");
//    std::string content((std::istreambuf_iterator<char>(file)),
//                        std::istreambuf_iterator<char>());
//    Source src("language_test.reflex", std::move(content));
//    Lexer lexer(&src, src.getContent(), getTokenDescription(), getKeywordDescription());
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
