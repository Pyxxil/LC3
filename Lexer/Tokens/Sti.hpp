#ifndef TOKEN_STI_HPP
#define TOKEN_STI_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Sti : public Token {
public:
  explicit Sti(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Sti(const Sti &) = default;
  Sti(Sti &&) = default;

  Sti &operator=(const Sti &) = default;
  Sti &operator=(Sti &&) = default;

  TokenType token_type() const final { return STI; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif