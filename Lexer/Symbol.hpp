#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>

namespace Lexer {

struct Symbol {
  Symbol() = default;
  Symbol(std::string t_name, uint16_t t_address, std::string t_file,
         size_t t_column, size_t t_line)
      : m_name(std::move(t_name)), m_address(t_address),
        m_file(std::move(t_file)), m_column(t_column), m_line(t_line) {}

  Symbol(const Symbol &) = default;
  Symbol(Symbol &&) = default;

  Symbol &operator=(const Symbol &) = default;
  Symbol &operator=(Symbol &&) = default;

  ~Symbol() = default;

  constexpr const std::string &name() const { return m_name; }
  constexpr const std::string &file() const { return m_file; }

  constexpr uint16_t address() const { return m_address; }
  constexpr size_t column() const { return m_column; }
  constexpr size_t line() const { return m_line; }

  const std::string m_name{};
  const uint16_t m_address{};
  const std::string m_file{};
  const size_t m_column{};
  const size_t m_line{};
};

} // namespace Lexer

#endif
