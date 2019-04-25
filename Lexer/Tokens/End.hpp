#ifndef TOKEN_END_HPP
#define TOKEN_END_HPP

#include "Token.hpp"

namespace Lexer::Token {

class End : public Token {
public:
  explicit End(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file) {}

  End(const End &) = default;
  End(End &&) = default;

  End &operator=(const End &) = default;
  End &operator=(End &&) = default;

  TokenType token_type() const final { return END; }

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif