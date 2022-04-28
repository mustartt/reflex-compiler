#include <iostream>
#include <SourceManager.h>
#include <Lexer.h>
#include <Parser.h>
#include <ASTContext.h>
#include <AstPrinter.h>
#include <LexicalContext.h>

int main(int argc, char *argv[]) {
    using namespace reflex;

    SourceManager srcManager;
    auto &file = srcManager.open("language_test.reflex");

    ASTContext astContext;
    Lexer lexer(file, file.content(), getTokenDescription(), getKeywordDescription());
    Parser parser(astContext, lexer);
//    try {
//    } catch (UnrecoverableError &err) {
//        std::cout << err.getErrorLocation()->getLocationString() << std::endl;
//        err.getErrorLocation()->printSourceRegion(std::cout, true);
//        std::cout << err.getErrorMessage() << std::endl;
//    }

    auto astRoot = parser.parseCompilationUnit();

    AstPrinter printer(std::cout);
    printer.visit(*astRoot);

    LexicalContext context;
    auto lexRoot = context.createGlobalScope(astRoot);
    auto vardecl = dynamic_cast<VariableDecl *>(astRoot->getDecl(0));
    auto klsdecl = dynamic_cast<ClassDecl *>(astRoot->getDecl(1));
    auto fundecl = dynamic_cast<FunctionDecl *>(astRoot->getDecl(2));

    lexRoot->addScopeMember(vardecl->getDeclname(), nullptr);
    lexRoot->addScopeMember(klsdecl->getDeclname(), nullptr, context.createCompositeScope(klsdecl, lexRoot));

    context.dump(std::cout, lexRoot);

    return 0;
}
