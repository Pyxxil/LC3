#ifndef TOKEN_SET_HPP
#define TOKEN_SET_HPP

#include "../Token.hpp"

namespace Lexer {
namespace Token {
#ifdef ADDONS
class Set : public Token {
public:
  explicit Set(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::IMMEDIATE) |
                                   Match(TokenType::LABEL)})) {}

  Set(const Set &) = default;
  Set(Set &&) noexcept = default;

  Set &operator=(const Set &) = default;
  Set &operator=(Set &&) noexcept = default;

  TokenType tokenType() const final { return SET; }

  void assemble(int16_t &programCounter, size_t width, const std::string &symbol) override { }

  word memoryRequired() const override {
    const auto immediateValue =
        static_cast<Immediate *>(operands()[1].get())->value();
    return static_cast<word>((immediateValue > 15 || immediateValue < -16) ? 3
                                                                           : 2);
  }
};
#endif
} // namespace Token
} // namespace Lexer

#endif