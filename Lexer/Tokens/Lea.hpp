#ifndef TOKEN_LEA_HPP
#define TOKEN_LEA_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Lea : public Token {
public:
  explicit Lea(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Lea(const Lea &) = default;
  Lea(Lea &&) = default;

  Lea &operator=(const Lea &) = default;
  Lea &operator=(Lea &&) = default;

  TokenType token_type() const final { return LEA; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }

private:
};

} // namespace Lexer::Token

#endif