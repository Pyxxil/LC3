#ifndef TOKEN_IMMEDIATE_HPP
#define TOKEN_IMMEDIATE_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Immediate : public Token {
public:
  explicit Immediate(std::string t, size_t tLine, size_t tColumn,
                     const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  Immediate(const Immediate &) = default;
  Immediate(Immediate &&) = default;

  Immediate &operator=(const Immediate &) = default;
  Immediate &operator=(Immediate &&) = default;

  TokenType tokenType() const final { return IMMEDIATE; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbol) override {}

  int16_t value() const { return mValue; }

protected:
  int16_t mValue{};
};
} // namespace Token
} // namespace Lexer

#endif
