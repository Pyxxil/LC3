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

  [[nodiscard]] constexpr const auto &name() const { return m_name; }
  [[nodiscard]] constexpr const auto &file() const { return m_file; }

  [[nodiscard]] constexpr auto address() const { return m_address; }
  [[nodiscard]] constexpr auto column() const { return m_column; }
  [[nodiscard]] constexpr auto line() const { return m_line; }

  const std::string m_name{};
  const uint16_t m_address{};
  const std::string m_file{};
  const size_t m_column{};
  const size_t m_line{};
};

} // namespace Lexer

#endif
