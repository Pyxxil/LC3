#ifndef TOKEN_INCLUDE_HPP
#define TOKEN_INCLUDE_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Include : public Token {
public:
  Include(const std::string &t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::STRING)})) {}

  Include(const Include &) = default;
  Include(Include &&) = default;

  Include &operator=(const Include &) = default;
  Include &operator=(Include &&) = default;

  TokenType token_type() const final { return INCLUDE; }

  word memory_required() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif