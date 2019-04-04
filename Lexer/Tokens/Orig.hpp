#ifndef TOKEN_ORIG_HPP
#define TOKEN_ORIG_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Orig : public Token {
public:
  explicit Orig(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(1, {Match(TokenType::IMMEDIATE)})) {}

  Orig(const Orig &) = default;
  Orig(Orig &&) = default;

  Orig &operator=(const Orig &) = default;
  Orig &operator=(Orig &&) = default;

  TokenType token_type() const final { return ORIG; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {

    program_counter = static_cast<Immediate *>(operands()[0].get())->value();
    set_assembled(AssembledToken(
        program_counter,
        fmt::format(
            "(0000) {0:0>4X} {0:0>16b} ({1: >4d}) {2: <{3}s} .ORIG 0x{0:0>4X}",
            program_counter, line(), std::string{}, width)));
  }

  word memory_required() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif