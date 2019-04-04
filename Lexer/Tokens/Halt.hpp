#ifndef TOKEN_HALT_HPP
#define TOKEN_HALT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Halt : public Token {
public:
  explicit Halt(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file) {}

  Halt(const Halt &) = default;
  Halt(Halt &&) = default;

  Halt &operator=(const Halt &) = default;
  Halt &operator=(Halt &&) = default;

  TokenType token_type() const final { return HALT; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        0xF025,
        fmt::format(
            "({0:0>4X}) F025 1111000000100101 ({1: >4d}) {2: <{3}s} HALT",
            program_counter++, line(),
            sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif