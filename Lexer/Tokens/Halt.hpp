#ifndef TOKEN_HALT_HPP
#define TOKEN_HALT_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Halt : public Token {
public:
  explicit Halt(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file);

  Halt(const Halt &) = default;
  Halt(Halt &&) = default;

  Halt &operator=(const Halt &) = default;
  Halt &operator=(Halt &&) = default;

  TokenType token_type() const final { return HALT; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif