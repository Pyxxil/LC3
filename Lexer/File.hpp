#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <vector>

namespace Lexer {

struct Position {
public:
  Position(size_t column = 0, size_t line = 0);

  Position(const Position &other) = default;
  Position(Position &&other) noexcept = default;

  Position &operator=(const Position &other) = default;
  Position &operator=(Position &&other) = default;

  ~Position() = default;

  size_t column() const;
  size_t line() const;

  void inc_column();
  void set_column(size_t column);
  void inc_line();

  size_t m_column;
  size_t m_line;
};

class File {
public:
  File() = default;
  explicit File(std::string file_name);

  File(const File &other) = default;
  File(File &&other) = default;

  File &operator=(const File &other) = default;
  File &operator=(File &&other) = default;

  ~File() = default;

  const std::string &name() const { return m_file_name; }
  const Position &position() const { return m_position; }
  std::string line() const;

  void set_column(size_t column);

  bool next_line();

  void next_column();
  const std::string &line(size_t l) const;

  bool is_failure() const;

private:
  std::string m_file_name;
  std::ifstream m_file;
  Position m_position{};
  std::vector<std::string> lines;
};
} // namespace Lexer

#endif
