#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>

namespace Lexer {

class Position {
public:
  Position(std::size_t column = 0, std::size_t line = 0)
      : mColumn(column), mLine(line) {}

  Position(const Position &other) = default;
  Position(Position &&other) noexcept = default;

  Position &operator=(const Position &other) = default;
  Position &operator=(Position &&other) = default;

  ~Position() = default;

  std::size_t column() const { return mColumn; }
  std::size_t line() const { return mLine; }

  void incColumn() { ++mColumn; }
  void setColumn(std::size_t column) { mColumn = column; }
  void incLine() { ++mLine; }

private:
  std::size_t mColumn;
  std::size_t mLine;
};

class File {
public:
  File() = default;
  explicit File(std::string file_name)
      : mFileName(std::move(file_name)), mFile() {
    mFile.open(mFileName);
  }

  File(const File &other) = default;
  File(File &&other) = default;

  File &operator=(const File &other) = default;
  File &operator=(File &&other) = default;

  ~File() = default;

  const std::string &name() const { return mFileName; }
  const Position &position() const { return mPosition; }
  std::string line() const {
    if (lines.size() <= position().line() - 1)
      return "";
    return lines[position().line() - 1];
  }

  void setColumn(std::size_t column) { mPosition.setColumn(column); }

  bool nextLine() {
    mPosition.incLine();
    std::string currentLine;
    if (std::getline(mFile, currentLine)) {
      lines.emplace_back(currentLine);
      return true;
    }
    return false;
  }
  void nextColumn() { mPosition.incColumn(); }
  const std::string& line(size_t l) const {
    return lines[l];
  }

  bool isFailure() const { return mFile.fail() || !mFile.is_open(); }

private:
  std::string mFileName;
  std::ifstream mFile;
  Position mPosition{};
  std::vector<std::string> lines;
};
} // namespace Lexer

#endif