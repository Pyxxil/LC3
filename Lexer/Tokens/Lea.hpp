#ifndef TOKEN_LEA_HPP
#define TOKEN_LEA_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Lea : public Token {
public:
  explicit Lea(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::LABEL) |
                                   Match(TokenType::IMMEDIATE)})) {}

  Lea(const Lea &) = default;
  Lea(Lea &&) noexcept = default;

  Lea &operator=(const Lea &) = default;
  Lea &operator=(Lea &&) noexcept = default;

  TokenType tokenType() const final { return LEA; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0xE000;

    uint16_t DR = static_cast<Register *>(ops[0].get())->reg() << 9;

    bin |= DR;

    if (TokenType::IMMEDIATE == ops[1]->tokenType()) {
      bin |= ((static_cast<int16_t>(
                   static_cast<Immediate *>(ops[1].get())->value() & 0x1FF)
               << 7) >>
              7);
    } else {
      auto label = std::find_if(symbols.begin(), symbols.end(),
                                [&token = ops[1]->getToken()](const auto &sym) {
                                  return sym.second.name() == token;
                                });
      if (label != symbols.end()) {
        bin |=
            (static_cast<int16_t>(static_cast<int16_t>(label->second.address() -
                                                       (programCounter + 1))
                                  << 7) >>
             7) &
            0x1FF;
      } else {
        DEBUG("Couldn't find label '{}'", ops[1]->getToken());
      }
    }

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

// 3020
    setAssembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} LEA R{5:d} "
            "{6:s}",
            programCounter++, bin, line(),
            sym == symbols.end() ? "" : sym->second.name(), width, DR >> 9,
            TokenType::IMMEDIATE == ops[1]->tokenType()
                ? fmt::format("#{:d}",
                              static_cast<Immediate *>(ops[1].get())->value())
                : ops[1]->getToken())));
  }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif