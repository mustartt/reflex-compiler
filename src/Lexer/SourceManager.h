//
// Created by Admininstrator on 2022-04-22.
//

#ifndef REFLEX_SRC_LEXER_SOURCEMANAGER_H_
#define REFLEX_SRC_LEXER_SOURCEMANAGER_H_

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <istream>
#include <ostream>
#include <fstream>
#include <memory>

namespace reflex {

class InvalidSourceLocationError : public std::runtime_error {
  public:
    explicit InvalidSourceLocationError(const std::string &msg)
        : std::runtime_error("Invalid Source Location: " + msg) {}
};

class SourceError : public std::runtime_error {
  public:
    explicit SourceError(const std::string &msg)
        : std::runtime_error("Source Error: " + msg) {}
};

class SourceLocation;

class SourceManager {
  public:

    class SourceFile {
      public:
        SourceFile(std::string filename, std::istream &is) : filename{std::move(filename)} {
            std::string line;
            while (getline(is, line)) {
                source.emplace_back(std::move(line));
            }
        }

        [[nodiscard]] std::string content() const {
            std::string file;
            for (const auto &line: source) {
                file.append(line);
            }
            return file;
        }

        /// @param line line number representing the line number [1, n]
        [[nodiscard]] const std::string &line(size_t line) const {
            if (line == 0 || line > source.size())
                throw InvalidSourceLocationError{
                    "line " + std::to_string(line) +
                        " out of bound, expected 1 to " + std::to_string(source.size())
                };
            return source[line - 1];
        }

        SourceLocation &createSourceLocation(size_t startline, size_t startcol,
                                             size_t endline, size_t endcol) {
            locationContext.push_back(std::make_unique<SourceLocation>(
                *this, startline, startcol, endline, endcol)
            );
            return *locationContext.back().get();
        }
      private:
        std::string filename;
        std::vector<std::string> source;
        std::vector<std::unique_ptr<SourceLocation>> locationContext;
    };

    SourceFile &open(const std::string &filename) {
        std::ifstream file{filename};
        if (!file) throw SourceError{"Cannot open " + filename};
        if (fileContext.contains(filename)) throw SourceError{"File " + filename + " already opened"};
        fileContext.try_emplace(filename, filename, file);
        return fileContext.at(filename);
    }

  private:
    std::map<std::string, SourceFile> fileContext;
};

class SourceLocation {
  public:
    SourceLocation(SourceManager::SourceFile &parent,
                   size_t startline, size_t startcol,
                   size_t endline, size_t endcol)
        : parent(parent), startline(startline), startcol(startcol),
          endline(endline), endcol(endcol) {}
    SourceLocation(const SourceLocation &) = delete;
    SourceLocation(SourceLocation &&) = default;

    void printSourceRegion(std::ostream &os, bool underline = false) const {
        for (size_t line = startline; line <= endline; ++line) {
            os << parent.line(line) << std::endl;
            if (underline) {
                for (size_t col = 1;
                     col <= parent.line(line).size(); ++col) {
                    if ((line == startline && col < startcol) ||
                        (line == endline && col > endcol)) {
                        os << ' ';
                    } else {
                        os << '~';
                    }
                }
            }
            os << std::endl;
        }
    }
  private:
    SourceManager::SourceFile &parent;

    size_t startline;
    size_t startcol;
    size_t endline;
    size_t endcol;
};

}

#endif //REFLEX_SRC_LEXER_SOURCEMANAGER_H_
