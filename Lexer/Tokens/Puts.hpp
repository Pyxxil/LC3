#ifndef TOKEN_PUTS_HPP
#define TOKEN_PUTS_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Puts : public Token {
public:
  Puts(std::string t, size_t t_line, size_t t_column, const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file) {}

  Puts(const Puts &) = default;
  Puts(Puts &&) = default;

  Puts &operator=(const Puts &) = default;
  Puts &operator=(Puts &&) = default;

  TokenType token_type() const final { return PUTS; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        0xF022,
        fmt::format(
            "({0:0>4X}) F022 1111000000100010 ({1: >4d}) {2: <{3}s} PUTS",
            program_counter++, line(),
            sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memory_required() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif