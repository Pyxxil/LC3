#ifndef TOKEN_JMP_HPP
#define TOKEN_JMP_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Jmp : public Token {
public:
  explicit Jmp(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Jmp(const Jmp &) = default;
  Jmp(Jmp &&) = default;

  Jmp &operator=(const Jmp &) = default;
  Jmp &operator=(Jmp &&) = default;

  TokenType token_type() const final { return JMP; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif