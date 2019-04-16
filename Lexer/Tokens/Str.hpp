#ifndef TOKEN_STR_HPP
#define TOKEN_STR_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Str : public Token {
public:
  explicit Str(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Str(const Str &) = default;
  Str(Str &&) = default;

  Str &operator=(const Str &) = default;
  Str &operator=(Str &&) = default;

  TokenType token_type() const final { return STR; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif