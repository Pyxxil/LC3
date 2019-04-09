#ifndef TOKEN_PUTSP_HPP
#define TOKEN_PUTSP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Putsp : public Token {
public:
  Putsp(std::string t, size_t t_line, size_t t_column, const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file) {}

  Putsp(const Putsp &) = default;
  Putsp(Putsp &&) = default;

  Putsp &operator=(const Putsp &) = default;
  Putsp &operator=(Putsp &&) = default;

  TokenType token_type() const final { return PUTSP; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        0xF024,
        fmt::format(
            "({0:0>4X}) F024 1111000000100100 ({1: >4d}) {2: <{3}s} PUTSP",
            program_counter++, line(),
            sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memory_required() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif