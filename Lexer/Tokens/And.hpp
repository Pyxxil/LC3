#ifndef TOKEN_AND_HPP
#define TOKEN_AND_HPP

#include "Token.hpp"

namespace Lexer::Token {
class And : public Token {
public:
  explicit And(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  And(const And &) = default;
  And(And &&) = default;

  And &operator=(const And &) = default;
  And &operator=(And &&) = default;

  TokenType token_type() const final { return AND; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif