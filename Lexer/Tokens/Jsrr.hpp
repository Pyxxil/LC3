#ifndef TOKEN_JSRR_HPP
#define TOKEN_JSRR_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Jsrr : public Token {
public:
  explicit Jsrr(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file);

  Jsrr(const Jsrr &) = default;
  Jsrr(Jsrr &&) = default;

  Jsrr &operator=(const Jsrr &) = default;
  Jsrr &operator=(Jsrr &&) = default;

  TokenType token_type() const final { return JSRR; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif