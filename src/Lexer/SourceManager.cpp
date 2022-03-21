//
// Created by henry on 2022-03-20.
//

#include <sstream>
#include "SourceManager.h"

namespace reflex {

InvalidSourceLocation::InvalidSourceLocation(const std::string &arg) : runtime_error(arg) {}

Source::Source(std::string filename, std::string content)
    : filename(std::move(filename)), content(std::move(content)) {}

std::pair<std::string, size_t> Source::getSurroundingRegion(size_t line, size_t col, size_t size) {
    int pos = 0;
    std::string current;
    std::stringstream stream(content);
    for (int i = 0; i < line; ++i) {
        if (!std::getline(stream, current)) {
            throw InvalidSourceLocation(
                "Invalid Source Location: line out of bound " + std::to_string(line) + "."
            );
        }
    }

    return {current, col};
}

}

