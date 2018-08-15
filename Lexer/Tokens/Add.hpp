#ifndef TOKEN_ADD_HPP
#define TOKEN_ADD_HPP

#include <memory>

#include "../Token.hpp"
#include "Register.hpp"

#include "Immediate.hpp"
#include "fmt/ostream.h"

namespace Lexer {
namespace Token {
class Add : public Token {
public:
  explicit Add(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(
                  2,
                  {Match(TokenType::REGISTER), Match(TokenType::REGISTER),
                   Match(TokenType::REGISTER) | Match(TokenType::IMMEDIATE)},
                  3)) {}

  Add(const Add &) = default;
  Add(Add &&) noexcept = default;

  Add &operator=(const Add &) = default;
  Add &operator=(Add &&) noexcept = default;

  TokenType tokenType() const final { return ADD; }

  void assemble(int16_t &programCounter, size_t width,
                const std::string &symbol) override {
    const std::vector<std::unique_ptr<Token>> &ops = operands();

    uint16_t bin = 0x1000;

    uint16_t DR = static_cast<Register *>(ops[0].get())->reg() << 9;
    uint16_t SR1 = static_cast<Register *>(ops[1].get())->reg() << 6;

    bin = bin | DR | SR1;

    if (TokenType::REGISTER == ops[2]->tokenType()) {
      bin |= static_cast<Register *>(ops[2].get())->reg();
    } else {
      bin |= 0x20 | (static_cast<Immediate *>(ops[2].get())->value() & 0x1F);
    }

    setAssembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2:d}) {3: <{4}s} ADD R{5:d} "
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