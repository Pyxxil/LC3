#ifndef TOKEN_STRING_HPP
#define TOKEN_STRING_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class String : public Token {
public:
  explicit String(std::string s) : Token(std::move(s), Requirements()) {}

  String(const String &) = default;
  String(String &&) noexcept = default;

  String &operator=(const String &) = default;
  String &operator=(String &&) noexcept = default;

  ~String() override = default;

  Token_Type tokenType() const final { return STRING; }

private:
};
} // namespace Token
} // namespace Lexer

#endif
