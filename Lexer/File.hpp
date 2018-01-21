#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>

namespace Lexer {

class Position {
public:
  Position(std::size_t column = 0, std::size_t line = 0)
      : m_column(column), m_line(line) {}

  Position(const Position &other) = default;
  Position(Position &&other) noexcept = default;

  Position &operator=(const Position &other) = default;
  Position &operator=(Position &&other) = default;

  ~Position() = default;

  std::size_t column() const { return m_column; }
  std::size_t line() const { return m_line; }

  void inc_column() { ++m_column; }
  void set_column(std::size_t column) { m_column = column; }
  void inc_line() { ++m_line; }

private:
  std::size_t m_column;
  std::size_t m_line;
};

class File {
public:
  File() = default;
  explicit File(std::string file_name)
      : m_file_name(std::move(file_name)), m_file(m_file_name) {}

  File(const File &other) = default;
  File(File &&other) = default;

  File &operator=(const File &other) = default;
  File &operator=(File &&other) = default;

  ~File() = default;

  const std::string &name() const { return m_file_name; }
  const Position &position() const { return m_position; }
  std::string line() const {
    if (lines.size() <= position().line() - 1)
      return "";
    return lines[position().line() - 1];
  }

  void set_column(std::size_t column) { m_position.set_column(column); }

  bool next_line() {
    m_position.inc_line();
    std::string current_line;
    if (std::getline(m_file, current_line)) {
      lines.emplace_back(current_line);
      return true;
    }
    return false;
  }
  void next_column() { m_position.inc_column(); }

private:
  std::string m_file_name;
  std::ifstream m_file;
  Position m_position{};
  std::vector<std::string> lines;
};
} // namespace Lexer

#endif