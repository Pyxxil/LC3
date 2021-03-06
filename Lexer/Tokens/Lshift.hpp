#ifndef TOKEN_LSHIFT_HPP
#define TOKEN_LSHIFT_HPP

#include "Token.hpp"

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
class Lshift : public Token {
public:
  Lshift(std::string t, size_t t_line, size_t t_column, const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::IMMEDIATE)})) {}

  Lshift(const Lshift &) = default;
  Lshift(Lshift &&) = default;

  Lshift &operator=(const Lshift &) = default;
  Lshift &operator=(Lshift &&) = default;

  TokenType token_type() const final { return LSHIFT; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0x1000;
    uint16_t reg = static_cast<Register *>(ops.front().get())->reg();
    bin |= (reg << 9) | (reg << 6) | reg;

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    for (auto i = 0; i < static_cast<Immediate *>(ops[1].get())->value(); i++) {
      as_assembled.emplace_back(
          bin,
          fmt::format(
              "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} ADD R{5:d} "
              "R{5:d} R{5:d}",
              programCounter++, bin, line(),
              sym == symbols.end() ? "" : sym->second.name(), width, reg));
    }
  }

  word memory_required() const override {
    return static_cast<word>(
        static_cast<Immediate *>(operands().back().get())->value());
  }
}; // namespace Token
} // namespace Token
} // namespace Lexer

#endif