#ifndef TOKEN_OUT_HPP
#define TOKEN_OUT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Out : public Token {
public:
  Out(const std::string &t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  Out(const Out &) = default;
  Out(Out &&) = default;

  Out &operator=(const Out &) = default;
  Out &operator=(Out &&) = default;

  TokenType token_type() const final { return OUT; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    set_assembled(AssembledToken(
        0xF021,
        fmt::format(
            "({0:0>4X}) F021 1111000000100001 ({1: >4d}) {2: <{3}s} OUT",
            programCounter++, line(),
            sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memory_required() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif