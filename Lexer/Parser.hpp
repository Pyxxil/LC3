#ifndef PARSER_HPP
#define PARSER_HPP

#include <map>

#include "Lexer.hpp"
#include "Tokens.hpp"

namespace {
constexpr size_t DEFAULT_MAX_SYMBOL_LENGTH = 20;
}

namespace Parser {

class Parser {
public:
  explicit Parser(std::vector<std::unique_ptr<Lexer::Token::Token>> tokens);

  void parse();

  void error() { ++error_count; }
  static void warning() {}

  [[nodiscard]] const auto &tokens() const { return m_tokens; }
  [[nodiscard]] const auto &symbols() const { return m_symbols; }

  [[nodiscard]] auto is_okay() const { return error_count == 0; }

  [[nodiscard]] auto symbol_length() { return longest_symbol_length; }

private:
  std::vector<std::unique_ptr<Lexer::Token::Token>> m_tokens{};
  std::map<std::string, Lexer::Symbol> m_symbols{};
  size_t error_count{0};
  int longest_symbol_length{DEFAULT_MAX_SYMBOL_LENGTH};
};

} // namespace Parser

#endif
