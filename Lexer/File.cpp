#include "File.hpp"

namespace Lexer {

Position::Position(size_t column, size_t line)
    : m_column(column), m_line(line) {}

size_t Position::column() const { return m_column; }

size_t Position::line() const { return m_line; }

void Position::inc_column() { ++m_column; }

void Position::set_column(size_t column) { m_column = column; }

void Position::inc_line() { ++m_line; }

File::File(std::string file_name)
    : m_file_name(std::move(file_name)), m_file() {
  m_file.open(m_file_name);

  if (!is_failure()) {
    std::string line;
    while (std::getline(m_file, line)) {
      lines.emplace_back(std::move(line));
    }
  }
}

std::string File::line() const {
  if (lines.size() <= position().line() - 1)
    return {};
  return lines[position().line() - 1];
}

void File::set_column(size_t column) { m_position.set_column(column); }

bool File::next_line() {
  m_position.inc_line();
  return m_position.line() < lines.size();
}

void File::next_column() { m_position.inc_column(); }

const std::string &File::line(size_t l) const { return lines[l]; }

bool File::is_failure() const {
  return (m_file.fail() && lines.empty()) || !m_file.is_open();
}

} // namespace Lexer