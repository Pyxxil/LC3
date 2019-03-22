#ifndef TOKEN_HALT_HPP
#define TOKEN_HALT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Halt : public Token {
public:
  explicit Halt(const std::string &t, size_t tLine, size_t tColumn,
                const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  Halt(const Halt &) = default;
  Halt(Halt &&) = default;

  Halt &operator=(const Halt &) = default;
  Halt &operator=(Halt &&) = default;

  TokenType token_type() const final { return HALT; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    set_assembled(AssembledToken(
        0xF025,
        fmt::format(
            "({0:0>4X}) F025 1111000000100101 ({1: >4d}) {2: <{3}s} HALT",
            programCounter++, line(),
            sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif