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
#include <unordered_set>
#include <functional>

namespace reflex {

class InvalidSourceLocationError : public std::runtime_error {
  public:
    explicit InvalidSourceLocationError(const std::string &msg);
};

class SourceError : public std::runtime_error {
  public:
    explicit SourceError(const std::string &msg);
};

class SourceFile;
class SourceLocation {
  public:
    SourceLocation(SourceFile &parent,
                   size_t startline, size_t startcol,
                   size_t endline, size_t endcol);
    SourceLocation(const SourceLocation &) = delete;
    SourceLocation(SourceLocation &&) = default;

    void printSourceRegion(std::ostream &os, bool underline = false) const;
    [[nodiscard]] std::string getLocationString() const;
    [[nodiscard]] std::string getStringRepr() const;

    [[nodiscard]] std::pair<size_t, size_t> getStartLocation() const;
    [[nodiscard]] std::pair<size_t, size_t> getEndLocation() const;
    [[nodiscard]] SourceFile &getSource() const { return parent; }
    bool operator==(const SourceLocation &rhs) const;

  private:
    SourceFile &parent;

    size_t startline;
    size_t startcol;
    size_t endline;
    size_t endcol;
};

struct SourceLocationHash {
  std::size_t operator()(const SourceLocation &loc) const noexcept;
};

class SourceFile {
  public:
    SourceFile(std::string filename, std::istream &is);

    [[nodiscard]] std::string content() const;

    /// @param line line number representing the line number [1, n]
    [[nodiscard]] const std::string &line(size_t line) const;

    const SourceLocation &createSourceLocation(size_t startline, size_t startcol,
                                               size_t endline, size_t endcol);
    const SourceLocation &mergeSourceLocation(const SourceLocation &loc1, const SourceLocation &loc2);
    bool operator==(const SourceFile &rhs) const;

    [[nodiscard]] const std::string &getFilename() const;

  private:
    std::string filename;
    std::vector<std::string> source;
    std::unordered_set<SourceLocation, SourceLocationHash> locationContext;
};

class SourceManager {
  public:
    SourceFile &open(const std::string &filename);

    static const SourceLocation &mergeSourceLocation(const SourceLocation &loc1, const SourceLocation &loc2);
    static const SourceLocation &createSourceLocation(SourceFile &source,
                                                      size_t startline, size_t startcol,
                                                      size_t endline, size_t endcol);
  private:
    std::map<std::string, SourceFile> fileContext;
};

}

#endif //REFLEX_SRC_LEXER_SOURCEMANAGER_H_
