#ifndef TOKEN_PUTSP_HPP
#define TOKEN_PUTSP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Putsp : public Token {
public:
  Putsp(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  Putsp(const Putsp &) = default;
  Putsp(Putsp &&) = default;

  Putsp &operator=(const Putsp &) = default;
  Putsp &operator=(Putsp &&) = default;

  TokenType tokenType() const final { return PUTSP; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    setAssembled(AssembledToken(
        0xF024,
        fmt::format(
            "({0:0>4X}) F024 1111000000100100 ({1: >4d}) {2: <{3}s} PUTSP",
            programCounter++, line(),
            sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memoryRequired() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif