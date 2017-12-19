#ifndef TOKEN_LABEL_HPP
#define TOKEN_LABEL_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Label : public Token {
public:
  explicit Label(std::string s) : Token(std::move(s)) {}

  Label(const Label &) = default;
  Label(Label &&) noexcept = default;

  Label &operator=(const Label &) = default;
  Label &operator=(Label &&) noexcept = default;

  ~Label() override = default;

  Token_Type tokenType() const final { return LABEL; }

private:
};
} // namespace Token
} // namespace Lexer

#endif
