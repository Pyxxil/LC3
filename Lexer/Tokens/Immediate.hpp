#ifndef TOKEN_IMMEDIATE_HPP
#define TOKEN_IMMEDIATE_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Immediate : public Token {
public:
  explicit Immediate(std::string t, size_t t_line, size_t tColumn,
                     const std::string &t_file)
      : Token(std::move(t), t_line, tColumn, t_file) {}

  Immediate(const Immediate &) = default;
  Immediate(Immediate &&) = default;

  Immediate &operator=(const Immediate &) = default;
  Immediate &operator=(Immediate &&) = default;

  TokenType token_type() const final { return IMMEDIATE; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbol,
                const std::string &sym) override {}

  int16_t value() const { return m_value; }

protected:
  int16_t m_value{};
};

} // namespace Lexer::Token

#endif
