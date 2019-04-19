#ifndef TOKEN_LDI_HPP
#define TOKEN_LDI_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Ldi : public Token {
public:
  explicit Ldi(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Ldi(const Ldi &) = default;
  Ldi(Ldi &&) = default;

  Ldi &operator=(const Ldi &) = default;
  Ldi &operator=(Ldi &&) = default;

  TokenType token_type() const final { return LDI; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif