#ifndef TOKEN_LABEL_HPP
#define TOKEN_LABEL_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Label : public Token {
public:
  explicit Label(const std::string &s, size_t tLine, size_t tColumn,
                 const std::string &tFile)
      : Token(std::move(s), tLine, tColumn, tFile) {}

  Label(const Label &) = default;
  Label(Label &&) = default;

  Label &operator=(const Label &) = default;
  Label &operator=(Label &&) = default;

  TokenType token_type() const final { return LABEL; }

  word memory_required() const override { return 0_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif
