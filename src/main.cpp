#include <iostream>
#include <SourceManager.h>
#include <Lexer.h>
#include <Parser.h>
#include <ASTContext.h>
#include <AstPrinter.h>
#include <LexicalContext.h>
#include <TypeContext.h>
#include <LexicalContextForwardPass.h>
#include <LexicalContextDeclTypePass.h>
#include <SemanticAnalysisPass.h>

int main(int argc, char *argv[]) {
    using namespace reflex;

    SourceManager srcManager;
    auto &file = srcManager.open("language_test.reflex");

    ASTContext astContext;
    Lexer lexer(file, file.content(), getTokenDescription(), getKeywordDescription());
    Parser parser(astContext, lexer);
    try {
        auto astRoot = parser.parseCompilationUnit();

        LexicalContext lexicalContext;
        TypeContext typeContext;

        AstPrinter printer(std::cout);

        LexicalContextForwardPass lexicalPass(lexicalContext, typeContext);
        LexicalContextDeclTypePass typePass(typeContext);

        auto lexscope = lexicalPass.performPass(astRoot);
        typePass.performPass(astRoot);

        printer.visit(*astRoot);
        lexicalContext.dump(std::cout, lexscope);
        typeContext.dump(std::cout);

        std::cout << std::string(75, '=') << std::endl;

        SemanticAnalysisPass functionPass(typeContext, lexscope->resolve("main")->getChild(), astContext);
        functionPass.analyzeFunction(dynamic_cast<FunctionDecl *>(astRoot->getDecl(2)));

        printer.visit(*astRoot);
        lexicalContext.dump(std::cout, lexscope);
        typeContext.dump(std::cout);

    } catch (UnrecoverableError &err) {
        std::cout << err.getErrorLocation()->getLocationString() << std::endl;
        err.getErrorLocation()->printSourceRegion(std::cout, true);
        std::cout << err.getErrorMessage() << std::endl;
    }

    return 0;
}
