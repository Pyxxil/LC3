#ifndef TOKEN_RTI_HPP
#define TOKEN_RTI_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Rti : public Token {
public:
  explicit Rti(std::string t) : Token(std::move(t)) {}

  Rti(const Rti &) = default;
  Rti(Rti &&) noexcept = default;

  Rti &operator=(const Rti &) = default;
  Rti &operator=(Rti &&) noexcept = default;

  ~Rti() override = default;

  Token_Type tokenType() const final { return RTI; }

  void assemble() override { Token::assemble(); }

private:
};
} // namespace Token
} // namespace Lexer

#endif