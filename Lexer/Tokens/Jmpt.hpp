#ifndef TOKEN_JMPT_HPP
#define TOKEN_JMPT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Jmpt : public Token {
public:
  explicit Jmpt(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(1, {Match(TokenType::REGISTER)})) {}

  Jmpt(const Jmpt &) = default;
  Jmpt(Jmpt &&) = default;

  Jmpt &operator=(const Jmpt &) = default;
  Jmpt &operator=(Jmpt &&) = default;

  TokenType token_type() const final { return JMPT; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const uint16_t reg =
        static_cast<Register *>(operands().front().get())->reg() << 6;
    const uint16_t bin = static_cast<const uint16_t>(0xC001 | reg);

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    set_assembled(AssembledToken(
        bin, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                         "JMPT R{5:d}",
                         programCounter++, bin, line(),
                         sym == symbols.end() ? "" : sym->second.name(), width,
                         reg >> 6)));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif