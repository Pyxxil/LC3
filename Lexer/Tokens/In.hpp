#ifndef TOKEN_IN_HPP
#define TOKEN_IN_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class In : public Token {
public:
  explicit In(std::string t, size_t t_line, size_t t_column,
              const std::string &t_file)
      : Token(std::move(t), t_line, t_column,t_file) {}

  In(const In &) = default;
  In(In &&) = default;

  In &operator=(const In &) = default;
  In &operator=(In &&) = default;

  TokenType token_type() const final { return IN; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        0xF023,
        fmt::format("({0:0>4X}) F023 1111000000100011 ({1: >4d}) {2: <{3}s} IN",
                    program_counter++, line(),
                    sym == symbols.end() ? "" : sym->second.name(), width)));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif