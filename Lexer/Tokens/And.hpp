#ifndef TOKEN_AND_HPP
#define TOKEN_AND_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class And : public Token {
public:
  explicit And(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(
                  2,
                  {Match(TokenType::REGISTER), Match(TokenType::REGISTER),
                   Match(TokenType::REGISTER) | Match(TokenType::IMMEDIATE)},
                  3)) {}

  And(const And &) = default;
  And(And &&) noexcept = default;

  And &operator=(const And &) = default;
  And &operator=(And &&) noexcept = default;

  TokenType tokenType() const final { return AND; }

  void assemble(int16_t &programCounter, size_t width,
                const std::string &symbol) override {
    const std::vector<std::unique_ptr<Token>> &ops = operands();

    uint16_t bin{0x5000};

    const uint16_t DR = static_cast<Register *>(ops[0].get())->reg() << 9;
    const uint16_t SR1 = static_cast<Register *>(ops[1].get())->reg() << 6;

    bin = bin | DR | SR1;

    if (TokenType::REGISTER == ops[2]->tokenType()) {
      bin |= static_cast<Register *>(ops[2].get())->reg();
    } else {
      bin |= 0x20 | (static_cast<Immediate *>(ops[2].get())->value() & 0x1F);
    }

    setAssembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2:d}) {3: <{4}s} AND R{5:d} "
            "R{6:d} {7:s}",
            programCounter++, bin, line(), symbol, width, DR >> 9, SR1 >> 6,
            (bin & 0x20)
                ? fmt::format("#{:d}",
                              (static_cast<int16_t>(bin & 0x1F) << 11) >> 11)
                : fmt::format("R{:d}", bin & 0x7))));
  }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif