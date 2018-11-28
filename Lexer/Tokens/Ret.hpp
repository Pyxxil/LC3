#ifndef TOKEN_RET_HPP
#define TOKEN_RET_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ret : public Token {
public:
  Ret(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile, Requirements()) {}

  Ret(const Ret &) = default;
  Ret(Ret &&) = default;

  Ret &operator=(const Ret &) = default;
  Ret &operator=(Ret &&) = default;

  TokenType tokenType() const final { return RET; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const uint16_t bin = 0xC1C0;

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    setAssembled(AssembledToken(
        bin,
        fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                    "RET",
                    programCounter++, bin, line(),
                    sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memoryRequired() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif