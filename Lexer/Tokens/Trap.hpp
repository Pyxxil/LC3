#ifndef TOKEN_TRAP_HPP
#define TOKEN_TRAP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Trap : public Token {
public:
  Trap(std::string t, size_t t_line, size_t t_column, const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(1, {Match(TokenType::IMMEDIATE)})) {}

  Trap(const Trap &) = default;
  Trap(Trap &&) = default;

  Trap &operator=(const Trap &) = default;
  Trap &operator=(Trap &&) = default;

  TokenType token_type() const final { return TRAP; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();
    const uint16_t bin =
        static_cast<const uint16_t>(0xF000 | (static_cast<Immediate *>(ops.front().get())->value() & 0xFF));

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    set_assembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} TRAP x{5:X}",
            programCounter++, bin, line(),
            sym == symbols.end() ? "" : sym->second.name(), width,
            static_cast<Immediate *>(ops.front().get())->value() & 0xFF)));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif