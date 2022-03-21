#include <iostream>
#include <fstream>

#include "Lexer/SourceManager.h"
#include "Lexer/Token.h"
#include "Lexer/Lexer.h"

int main(int argc, char *argv[]) {
    using namespace reflex;
    std::ifstream file("test.reflex");
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    Source src("test.reflex", std::move(content));
    Lexer lexer(&src, src.getContent(), getTokenDescription(), getKeywordDescription());

    while (lexer.hasNext()) {
        auto tok = lexer.nextToken();
        if (tok.getTokenType().getValue() != TokenType::WhiteSpace) {
            std::cout << tok << std::endl;
        }
    }
}
