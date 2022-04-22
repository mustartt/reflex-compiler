//
// Created by henry on 2022-03-20.
//

#ifndef REFLEX_SRC_LEXER_SOURCE_H_
#define REFLEX_SRC_LEXER_SOURCE_H_

#include <string>
#include <unordered_map>

namespace reflex {

class InvalidSourceLocation : public std::runtime_error {
  public:
    explicit InvalidSourceLocation(const std::string &arg);
};

class Source {
    std::string filename;
    std::string content;
  public:
    Source(std::string filename, std::string content);
    std::pair<std::string, size_t> getSurroundingRegion(size_t line, size_t col, size_t size);

    [[nodiscard]] const std::string &getContent() const { return content; }
    [[nodiscard]] const std::string &getFilename() const { return filename; }
};

}

#endif //REFLEX_SRC_LEXER_SOURCE_H_
