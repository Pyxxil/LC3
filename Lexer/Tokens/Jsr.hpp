#ifndef TOKEN_JSR_HPP
#define TOKEN_JSR_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Jsr : public Token {
public:
  explicit Jsr(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Jsr(const Jsr &) = default;
  Jsr(Jsr &&) = default;

  Jsr &operator=(const Jsr &) = default;
  Jsr &operator=(Jsr &&) = default;

  TokenType token_type() const final { return JSR; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif