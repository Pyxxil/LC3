#ifndef TOKEN_REGISTER_HPP
#define TOKEN_REGISTER_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Register : public Token
{
public:
  explicit Register(std::string s)
    : Token(std::move(s), Requirements())
    , reg(static_cast<size_t>(s[1] - 0x30))
  {}

  Register(const Register&) = default;
  Register(Register&&) noexcept = default;

  Register& operator=(const Register&) = default;
  Register& operator=(Register&&) noexcept = default;

  ~Register() override = default;

  Token_Type tokenType() const final { return REGISTER; }

  void assemble() override { Token::assemble(); }

private:
  size_t reg{};
};
}
}

#endif
