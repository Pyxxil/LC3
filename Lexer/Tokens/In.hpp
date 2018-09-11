#ifndef TOKEN_IN_HPP
#define TOKEN_IN_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class In : public Token {
public:
  explicit In(std::string t, size_t tLine, size_t tColumn,
              const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile) {}

  In(const In &) = default;
  In(In &&) noexcept = default;

  In &operator=(const In &) = default;
  In &operator=(In &&) noexcept = default;

  TokenType tokenType() const final { return IN; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    setAssembled(AssembledToken(
        0xF023,
        fmt::format("({0:0>4X}) F023 1111000000100011 ({1: >4d}) {2: <{3}s} IN",
                    programCounter++, line(),
                    sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif