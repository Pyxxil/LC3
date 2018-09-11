#ifndef TOKEN_PUTS_HPP
#define TOKEN_PUTS_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Puts : public Token {
public:
  Puts(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  Puts(const Puts &) = default;
  Puts(Puts &&) noexcept = default;

  Puts &operator=(const Puts &) = default;
  Puts &operator=(Puts &&) noexcept = default;

  TokenType tokenType() const final { return PUTS; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    setAssembled(AssembledToken(
        0xF022,
        fmt::format(
            "({0:0>4X}) F022 1111000000100010 ({1: >4d}) {2: <{3}s} PUTS",
            programCounter++, line(),
            sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memoryRequired() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif