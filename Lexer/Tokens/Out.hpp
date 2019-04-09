#ifndef TOKEN_OUT_HPP
#define TOKEN_OUT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Out : public Token {
public:
  Out(std::string t, size_t t_line, size_t t_column, const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file) {}

  Out(const Out &) = default;
  Out(Out &&) = default;

  Out &operator=(const Out &) = default;
  Out &operator=(Out &&) = default;

  TokenType token_type() const final { return OUT; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        0xF021,
        fmt::format(
            "({0:0>4X}) F021 1111000000100001 ({1: >4d}) {2: <{3}s} OUT",
            program_counter++, line(),
            sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memory_required() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif