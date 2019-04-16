#ifndef TOKEN_PUTS_HPP
#define TOKEN_PUTS_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Puts : public Token {
public:
  Puts(std::string t, size_t t_line, size_t t_column,
       const std::string &t_file);

  Puts(const Puts &) = default;
  Puts(Puts &&) = default;

  Puts &operator=(const Puts &) = default;
  Puts &operator=(Puts &&) = default;

  TokenType token_type() const final { return PUTS; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif