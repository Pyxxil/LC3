#ifndef TOKEN_JSRR_HPP
#define TOKEN_JSRR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Jsrr : public Token {
public:
  explicit Jsrr(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(1, {Match(TokenType::REGISTER)})) {}

  Jsrr(const Jsrr &) = default;
  Jsrr(Jsrr &&) = default;

  Jsrr &operator=(const Jsrr &) = default;
  Jsrr &operator=(Jsrr &&) = default;

  TokenType token_type() const final { return JSRR; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    uint16_t bin = 0x4000;
    uint16_t reg = static_cast<Register *>(operands().front().get())->reg()
                   << 6;
    bin |= reg;

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        bin, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                         "JSRR R{}",
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