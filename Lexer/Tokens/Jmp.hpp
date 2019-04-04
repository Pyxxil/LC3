#ifndef TOKEN_JMP_HPP
#define TOKEN_JMP_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Jmp : public Token {
public:
  explicit Jmp(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(1, {Match(TokenType::REGISTER)})) {}

  Jmp(const Jmp &) = default;
  Jmp(Jmp &&) = default;

  Jmp &operator=(const Jmp &) = default;
  Jmp &operator=(Jmp &&) = default;

  TokenType token_type() const final { return JMP; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    uint16_t bin = 0xC000;
    uint16_t reg = static_cast<Register *>(operands().front().get())->reg()
                   << 6;
    bin |= reg;

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        bin, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                         "JMP R{5:d}",
                         program_counter++, bin, line(),
                         sym == symbols.end() ? "" : sym->second.name(), width,
                         reg >> 6)));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif