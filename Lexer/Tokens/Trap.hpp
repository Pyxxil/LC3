#ifndef TOKEN_TRAP_HPP
#define TOKEN_TRAP_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Trap : public Token {
public:
  Trap(std::string t, size_t t_line, size_t t_column,
       const std::string &t_file);

  Trap(const Trap &) = default;
  Trap(Trap &&) = default;

  Trap &operator=(const Trap &) = default;
  Trap &operator=(Trap &&) = default;

  TokenType token_type() const final { return TRAP; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif