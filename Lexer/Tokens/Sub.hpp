#ifndef TOKEN_SUB_HPP
#define TOKEN_SUB_HPP

#include "Token.hpp"

#include "Register.hpp"

namespace Lexer {
namespace Token {
class Sub : public Token {
public:
  explicit Sub(std::string t, size_t tLine, size_t tColumn,
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

  TokenType tokenType() const final { return SUB; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    size_t first_reg_idx = ops.size() - 2;
    size_t second_reg_idx = ops.size() - 1;

    if (static_cast<Register *>(ops[first_reg_idx].get())->reg() ==
        static_cast<Register *>(ops[second_reg_idx].get())->reg()) {
      And a("AND", line(), column(), file());
      a.addOperand(std::make_unique<Register>(
          fmt::format("R{:d}",
                      static_cast<Register *>(ops.front().get())->reg()),
          line(), column(), file()));
      a.addOperand(std::make_unique<Register>(
          fmt::format("R{:d}",
                      static_cast<Register *>(ops[first_reg_idx].get())->reg()),
          line(), column(), file()));
      a.addOperand(std::make_unique<Decimal>(
          fmt::format(
              "0", static_cast<Register *>(ops[second_reg_idx].get())->reg()),
          line(), column(), file()));
      a.assemble(programCounter, width, symbols);

      asAssembled = a.assembled();
    } else {
      const uint16_t DR = static_cast<Register *>(ops.front().get())->reg();
      const uint16_t SR1 =
          static_cast<Register *>(ops[first_reg_idx].get())->reg();
      const uint16_t SR2 =
          static_cast<Register *>(ops[second_reg_idx].get())->reg();

      Neg n(".NEG", line(), column(), file());
      n.addOperand(std::make_unique<Register>(fmt::format("R{:d}", SR2), line(),
                                              column(), file()));
      n.assemble(programCounter, width, symbols);

      Add a("ADD", line(), column(), file());
      a.addOperand(std::make_unique<Register>(fmt::format("R{:d}", DR), line(),
                                              column(), file()));
      a.addOperand(std::make_unique<Register>(fmt::format("R{:d}", SR1), line(),
                                              column(), file()));
      a.addOperand(std::make_unique<Register>(fmt::format("R{:d}", SR2), line(),
                                              column(), file()));
      a.assemble(programCounter, width, symbols);

      asAssembled = n.assembled();

      for (auto &&as : a.assembled()) {
        asAssembled.emplace_back(as);
      }

      if (DR != SR2) {
        Neg n(".NEG", line(), column(), file());
        n.addOperand(std::make_unique<Register>(fmt::format("R{:d}", SR2),
                                                line(), column(), file()));
        n.assemble(programCounter, width, symbols);

        for (auto &&as : n.assembled()) {
          asAssembled.emplace_back(as);
        }
      }
    }
  }

  word memoryRequired() const override {
    const size_t firstRegisterIndex = operands().size() - 2;
    const size_t secondRegisterIndex = operands().size() - 1;

    const auto secondRegister =
        static_cast<Register *>(operands()[secondRegisterIndex].get())->reg();

    if (static_cast<Register *>(operands()[firstRegisterIndex].get())->reg() ==
        secondRegister) {
      return 1_word;
    } else if (static_cast<Register *>(operands().front().get())->reg() !=
               secondRegister) {
      return 5_words;
    } else {
      return 3_words;
    }
  }
};
} // namespace Token
} // namespace Lexer

#endif