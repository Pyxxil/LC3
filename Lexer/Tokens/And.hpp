#ifndef TOKEN_AND_HPP
#define TOKEN_AND_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class And : public Token {
public:
  explicit And(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(
                  2,
                  {Match(TokenType::REGISTER), Match(TokenType::REGISTER),
                   Match(TokenType::REGISTER) | Match(TokenType::IMMEDIATE)},
                  3)) {}

  And(const And &) = default;
  And(And &&) = default;

  And &operator=(const And &) = default;
  And &operator=(And &&) = default;

  TokenType token_type() const final { return AND; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin{0x5000};

    uint16_t DR = static_cast<Register *>(ops[0].get())->reg();
    uint16_t SR1 = static_cast<Register *>(ops[1].get())->reg();
    if (ops.size() == 2) {
      bin |= (DR << 9) | (DR << 6) | SR1;
    } else if (TokenType::REGISTER == ops[2]->token_type()) {
      bin |=
          (DR << 9) | (SR1 << 6) | static_cast<Register *>(ops[2].get())->reg();
    } else {
      bin |= (DR << 9) | (SR1 << 6) | 0x20 |
             (static_cast<Immediate *>(ops[2].get())->value() & 0x1F);
    }

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} AND R{5:d} "
            "R{6:d} {7:s}",
            program_counter++, bin, line(),
            sym == symbols.end() ? "" : sym->second.name(), width,
            (bin & 0x0E00) >> 9, (bin & 0x01C0) >> 6,
            (bin & 0x20)
                ? fmt::format("#{:d}",
                              static_cast<int16_t>(
                                  (static_cast<int16_t>(bin & 0x1F) << 11)) >>
                                  11)
                : fmt::format("R{:d}", bin & 0x7))));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif