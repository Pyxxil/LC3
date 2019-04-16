#ifndef TOKEN_LDR_HPP
#define TOKEN_LDR_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Ldr : public Token {
public:
  explicit Ldr(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Ldr(const Ldr &) = default;
  Ldr(Ldr &&) = default;

  Ldr &operator=(const Ldr &) = default;
  Ldr &operator=(Ldr &&) = default;

  TokenType token_type() const final { return LDR; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif