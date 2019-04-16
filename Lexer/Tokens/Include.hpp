#ifndef TOKEN_INCLUDE_HPP
#define TOKEN_INCLUDE_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Include : public Token {
public:
  Include(std::string t, size_t t_line, size_t t_column,
          const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(1, {Match(TokenType::STRING)})) {}

  Include(const Include &) = default;
  Include(Include &&) = default;

  Include &operator=(const Include &) = default;
  Include &operator=(Include &&) = default;

  TokenType token_type() const final { return INCLUDE; }

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif