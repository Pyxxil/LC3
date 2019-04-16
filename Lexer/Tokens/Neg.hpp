#ifndef TOKEN_NEG_HPP
#define TOKEN_NEG_HPP

#include "Token.hpp"

namespace Lexer::Token {
#ifdef ADDONS
class Neg : public Token {
public:
  Neg(std::string t, size_t t_line, size_t t_column, const std::string &t_file);

  Neg(const Neg &) = default;
  Neg(Neg &&) = default;

  Neg &operator=(const Neg &) = default;
  Neg &operator=(Neg &&) = default;

  TokenType token_type() const final { return NEG; }

  // TODO: This could also just alter the NOT instructions lower bits to make
  // the simulator do 2's complement for us..
  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const final { return 2_words; }
};
#endif
} // namespace Lexer::Token

#endif