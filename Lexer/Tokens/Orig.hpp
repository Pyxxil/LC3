#ifndef TOKEN_ORIG_HPP
#define TOKEN_ORIG_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Orig : public Token {
public:
  explicit Orig(std::string t, size_t tLine, size_t tColumn,
                const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::IMMEDIATE)})) {}

  Orig(const Orig &) = default;
  Orig(Orig &&) = default;

  Orig &operator=(const Orig &) = default;
  Orig &operator=(Orig &&) = default;

  TokenType tokenType() const final { return ORIG; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {

    programCounter = static_cast<Immediate *>(operands()[0].get())->value();
    setAssembled(AssembledToken(
        programCounter,
        fmt::format(
            "(0000) {0:0>4X} {0:0>16b} ({1: >4d}) {2: <{3}s} .ORIG 0x{0:0>4X}",
            programCounter, line(), std::string{}, width)));
  }

  word memoryRequired() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif