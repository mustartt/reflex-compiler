//
// Created by Admininstrator on 2022-04-22.
//

#include "SourceManager.h"

namespace reflex {

InvalidSourceLocationError::InvalidSourceLocationError(const std::string &msg)
    : std::runtime_error("Invalid Source Location: " + msg) {}

SourceError::SourceError(const std::string &msg)
    : std::runtime_error("Source Error: " + msg) {}

SourceLocation::SourceLocation(SourceFile &parent, size_t startline, size_t startcol, size_t endline, size_t endcol)
    : parent(parent), startline(startline), startcol(startcol),
      endline(endline), endcol(endcol) {}

void SourceLocation::printSourceRegion(std::ostream &os, bool underline) const {
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

std::string SourceLocation::getStringRepr() const {
    return "<" + std::to_string(startline) + ":" + std::to_string(startcol)
        + ", " + std::to_string(endline) + ":" + std::to_string(endcol) + ">";
}

std::string SourceLocation::getLocationString() const {
    return parent.getFilename() + ": " + getStringRepr();
}

std::pair<size_t, size_t> SourceLocation::getStartLocation() const {
    return std::make_pair(startline, startcol);
}

std::pair<size_t, size_t> SourceLocation::getEndLocation() const {
    return std::make_pair(endline, endcol);
}

bool SourceLocation::operator==(const SourceLocation &rhs) const {
    return parent == rhs.parent &&
        startline == rhs.startline &&
        startcol == rhs.startcol &&
        endline == rhs.endline &&
        endcol == rhs.endcol;
}

std::size_t SourceLocationHash::operator()(const SourceLocation &loc) const noexcept {
    auto[startline, startcol] = loc.getStartLocation();
    auto[endline, endcol] = loc.getEndLocation();
    return std::hash<size_t>{}(startline)
        ^ std::hash<size_t>{}(startcol)
        ^ std::hash<size_t>{}(endline)
        ^ std::hash<size_t>{}(endcol);
}

SourceFile::SourceFile(std::string filename, std::istream &is) : filename{std::move(filename)} {
    std::string line;
    while (getline(is, line)) {
        source.emplace_back(std::move(line));
    }
}

std::string SourceFile::content() const {
    std::string file;
    for (const auto &line: source) {
        file.append(line);
    }
    return file;
}

const std::string &SourceFile::line(size_t line) const {
    if (line == 0 || line > source.size())
        throw InvalidSourceLocationError{
            "line " + std::to_string(line) +
                " out of bound, expected 1 to " + std::to_string(source.size())
        };
    return source[line - 1];
}

const SourceLocation &SourceFile::createSourceLocation(size_t startline,
                                                       size_t startcol,
                                                       size_t endline,
                                                       size_t endcol) {
    auto result = locationContext.emplace(*this, startline, startcol, endline, endcol);
    return *result.first;
}

using Loc = std::pair<size_t, size_t>;
template<class Compare = std::less<size_t>>
Loc &selectLocation(Loc &loc1, Loc &loc2, Compare cmp) {
    if (loc1.first == loc2.first) {
        return cmp(loc1.second, loc2.second) ? loc1 : loc2;
    }
    return cmp(loc1, loc2) ? loc1 : loc2;
}

const SourceLocation &SourceFile::mergeSourceLocation(const SourceLocation &loc1, const SourceLocation &loc2) {
    auto starting1 = loc1.getStartLocation();
    auto starting2 = loc2.getStartLocation();
    auto ending1 = loc1.getEndLocation();
    auto ending2 = loc2.getEndLocation();

    auto &[sline, scol] = selectLocation(starting1, starting2, std::less<>());
    auto &[eline, ecol] = selectLocation(ending1, ending2, std::greater<>());

    return createSourceLocation(sline, scol, eline, ecol);
}

bool SourceFile::operator==(const SourceFile &rhs) const {
    return filename == rhs.filename;
}

const std::string &SourceFile::getFilename() const {
    return filename;
}

SourceFile &SourceManager::open(const std::string &filename) {
    std::ifstream file{filename};
    if (!file) throw SourceError{"Cannot open " + filename};
    if (fileContext.contains(filename)) throw SourceError{"File " + filename + " already opened"};
    fileContext.try_emplace(filename, filename, file);
    return fileContext.at(filename);
}

const SourceLocation &
SourceManager::mergeSourceLocation(const SourceLocation &loc1, const SourceLocation &loc2) {
    return loc1.getSource().mergeSourceLocation(loc1, loc2);
}

const SourceLocation &
SourceManager::createSourceLocation(SourceFile &source,
                                    size_t startline, size_t startcol,
                                    size_t endline, size_t endcol) {
    return source.createSourceLocation(startline, startcol, endline, endcol);
}

}