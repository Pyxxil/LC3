#ifndef TOKEN_NOT_HPP
#define TOKEN_NOT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Not : public Token {
public:
  explicit Not(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(
                  1, {Match(TokenType::REGISTER), Match(TokenType::REGISTER)},
                  2)) {}

  Not(const Not &) = default;
  Not(Not &&) = default;

  Not &operator=(const Not &) = default;
  Not &operator=(Not &&) = default;

  TokenType token_type() const final { return NOT; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0x903F;

    uint16_t DR = static_cast<Register *>(ops[0].get())->reg() << 9;
    uint16_t SR1 = DR >> 3;

    if (ops.size() > 1) {
      SR1 = static_cast<Register *>(ops[1].get())->reg() << 6;
    }

    bin |= DR | SR1;

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} NOT R{5:d} "
            "R{6:d}",
            program_counter++, bin, line(),
            sym == symbols.end() ? "" : sym->second.name(), width, DR >> 9,
            SR1 >> 6)));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif