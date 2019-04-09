#ifndef TOKEN_RET_HPP
#define TOKEN_RET_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ret : public Token {
public:
  Ret(std::string t, size_t t_line, size_t t_column, const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file, Requirements()) {}

  Ret(const Ret &) = default;
  Ret(Ret &&) = default;

  Ret &operator=(const Ret &) = default;
  Ret &operator=(Ret &&) = default;

  TokenType token_type() const final { return RET; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const uint16_t bin = 0xC1C0;

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        bin,
        fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                    "RET",
                    program_counter++, bin, line(),
                    sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memory_required() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif