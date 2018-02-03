#ifndef TOKEN_RTI_HPP
#define TOKEN_RTI_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Rti : public Token {
public:
  Rti(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  Rti(const Rti &) = default;
  Rti(Rti &&) noexcept = default;

  Rti &operator=(const Rti &) = default;
  Rti &operator=(Rti &&) noexcept = default;

  TokenType tokenType() const final { return RTI; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif