#ifndef TOKEN_NOT_HPP
#define TOKEN_NOT_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Not : public Token {
public:
  explicit Not(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Not(const Not &) = default;
  Not(Not &&) = default;

  Not &operator=(const Not &) = default;
  Not &operator=(Not &&) = default;

  TokenType token_type() const final { return NOT; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif