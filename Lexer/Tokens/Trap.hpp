#ifndef TOKEN_TRAP_HPP
#define TOKEN_TRAP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Trap : public Token {
public:
  Trap(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::IMMEDIATE)})) {}

  Trap(const Trap &) = default;
  Trap(Trap &&) = default;

  Trap &operator=(const Trap &) = default;
  Trap &operator=(Trap &&) = default;

  TokenType tokenType() const final { return TRAP; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();
    const uint16_t bin =
        0xF000 | (static_cast<Immediate *>(ops.front().get())->value() & 0xFF);

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    setAssembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} TRAP x{5:X}",
            programCounter++, bin, line(),
            sym == symbols.end() ? "" : sym->second.name(), width,
            static_cast<Immediate *>(ops.front().get())->value() & 0xFF)));
  }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif