#ifndef TOKEN_SUB_HPP
#define TOKEN_SUB_HPP

#include "Token.hpp"

#include "Register.hpp"

namespace Lexer {
namespace Token {
class Sub : public Token {
public:
  explicit Sub(const std::string &t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2,
                           {Match(TokenType::REGISTER),
                            Match(TokenType::REGISTER),
                            Match(TokenType::REGISTER)},
                           3)) {}

  Sub(const Sub &) = default;
  Sub(Sub &&) = default;

  Sub &operator=(const Sub &) = default;
  Sub &operator=(Sub &&) = default;

  TokenType token_type() const final { return SUB; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    size_t first_reg_idx = ops.size() - 2;
    size_t second_reg_idx = ops.size() - 1;

    if (static_cast<Register *>(ops[first_reg_idx].get())->reg() ==
        static_cast<Register *>(ops[second_reg_idx].get())->reg()) {
      And a("AND", line(), column(), file());
      a.add_operand(std::make_unique<Register>(
          fmt::format("R{:d}",
                      static_cast<Register *>(ops.front().get())->reg()),
          line(), column(), file()));
      a.add_operand(std::make_unique<Register>(
          fmt::format("R{:d}",
                      static_cast<Register *>(ops[first_reg_idx].get())->reg()),
          line(), column(), file()));
      a.add_operand(std::make_unique<Decimal>(
          fmt::format(
              "0", static_cast<Register *>(ops[second_reg_idx].get())->reg()),
          line(), column(), file()));
      a.assemble(programCounter, width, symbols);

      as_assembled = a.assembled();
    } else {
      const uint16_t DR = static_cast<Register *>(ops.front().get())->reg();
      const uint16_t SR1 =
          static_cast<Register *>(ops[first_reg_idx].get())->reg();
      const uint16_t SR2 =
          static_cast<Register *>(ops[second_reg_idx].get())->reg();

      Neg n(".NEG", line(), column(), file());
      n.add_operand(std::make_unique<Register>(fmt::format("R{:d}", SR2),
                                               line(), column(), file()));
      n.assemble(programCounter, width, symbols);

      Add a("ADD", line(), column(), file());
      a.add_operand(std::make_unique<Register>(fmt::format("R{:d}", DR), line(),
                                               column(), file()));
      a.add_operand(std::make_unique<Register>(fmt::format("R{:d}", SR1),
                                               line(), column(), file()));
      a.add_operand(std::make_unique<Register>(fmt::format("R{:d}", SR2),
                                               line(), column(), file()));
      a.assemble(programCounter, width, symbols);

      as_assembled = n.assembled();

      for (auto &&as : a.assembled()) {
        as_assembled.emplace_back(as);
      }

      if (DR != SR2) {
        Neg n(".NEG", line(), column(), file());
        n.add_operand(std::make_unique<Register>(fmt::format("R{:d}", SR2),
                                                 line(), column(), file()));
        n.assemble(programCounter, width, symbols);

        for (auto &&as : n.assembled()) {
          as_assembled.emplace_back(as);
        }
      }
    }
  }

  word memory_required() const override {
    const size_t first_register_idx = operands().size() - 2;
    const size_t second_register_idx = operands().size() - 1;

    const auto second_register =
        static_cast<Register *>(operands()[second_register_idx].get())->reg();

    if (static_cast<Register *>(operands()[first_register_idx].get())->reg() ==
        second_register) {
      return 1_word;
    } else if (static_cast<Register *>(operands().front().get())->reg() !=
               second_register) {
      return 5_words;
    } else {
      return 3_words;
    }
  }
};
} // namespace Token
} // namespace Lexer

#endif