#ifndef TOKEN_LDR_HPP
#define TOKEN_LDR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ldr : public Token {
public:
  explicit Ldr(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(3, {Match(TokenType::REGISTER),
                               Match(TokenType::REGISTER),
                               Match(TokenType::IMMEDIATE)})) {}

  Ldr(const Ldr &) = default;
  Ldr(Ldr &&) = default;

  Ldr &operator=(const Ldr &) = default;
  Ldr &operator=(Ldr &&) = default;

  TokenType token_type() const final { return LDR; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0x6000;

    uint16_t DR = static_cast<Register *>(ops[0].get())->reg() << 9;
    uint16_t SR = static_cast<Register *>(ops[1].get())->reg() << 6;
    uint16_t offset = static_cast<uint16_t>((static_cast<int16_t>(
                               static_cast<int16_t>(
                                   static_cast<Immediate *>(ops[2].get())->value())
                               << 10) >>
                           10) &
                          0x3F);

    bin |= DR | SR;

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} LDR R{5:d} "
            "R{6:d} #{7:d}",
            program_counter++, bin, line(),
            sym == symbols.end() ? "" : sym->second.name(), width, DR >> 9,
            SR >> 6, static_cast<int16_t>(offset))));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif