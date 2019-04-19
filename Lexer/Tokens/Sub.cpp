#include "Sub.hpp"

#include "Add.hpp"
#include "And.hpp"
#include "Decimal.hpp"
#include "Neg.hpp"

namespace Lexer::Token {
#ifdef ADDONS
Sub::Sub(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(
          std::move(t), t_line, t_column, t_file,
          Requirements(2,
                       {Match(TokenType::REGISTER), Match(TokenType::REGISTER),
                        Match(TokenType::REGISTER)},
                       3)) {}

void Sub::assemble(uint16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  const size_t first_reg_idx = ops.size() - 2;
  const size_t second_reg_idx = ops.size() - 1;

  const uint16_t DR = static_cast<Register *>(ops.front().get())->reg();
  const uint16_t SR1 = static_cast<Register *>(ops[first_reg_idx].get())->reg();
  const uint16_t SR2 =
      static_cast<Register *>(ops[second_reg_idx].get())->reg();

  auto &&dest_reg = fmt::format("R{:d}", DR);
  auto &&first_reg = fmt::format("R{:d}", SR1);
  auto &&second_reg = fmt::format("R{:d}", SR2);

  if (SR1 == SR2) {
    And a("AND", line(), column(), file());
    a.add_operand(
        std::make_unique<Register>(dest_reg, line(), column(), file()));
    a.add_operand(
        std::make_unique<Register>(first_reg, line(), column(), file()));
    a.add_operand(
        std::make_unique<Decimal>(second_reg, line(), column(), file()));
    a.assemble(program_counter, width, symbols, sym);
    as_assembled = a.assembled();
  } else {
    Neg n(".NEG", line(), column(), file());
    n.add_operand(
        std::make_unique<Register>(second_reg, line(), column(), file()));
    n.assemble(program_counter, width, symbols, sym);

    as_assembled = n.assembled();

    Add a("ADD", line(), column(), file());
    a.add_operand(
        std::make_unique<Register>(dest_reg, line(), column(), file()));
    a.add_operand(
        std::make_unique<Register>(first_reg, line(), column(), file()));
    a.add_operand(
        std::make_unique<Register>(second_reg, line(), column(), file()));
    a.assemble(program_counter, width, symbols, std::string{});

    as_assembled.insert(std::end(as_assembled),
                        std::make_move_iterator(std::begin(a.assembled())),
                        std::make_move_iterator(std::end(a.assembled())));

    if (DR != SR2) {
      Neg n(".NEG", line(), column(), file());
      n.add_operand(
          std::make_unique<Register>(second_reg, line(), column(), file()));
      n.assemble(program_counter, width, symbols, std::string{});

      as_assembled.insert(std::end(as_assembled),
                          std::make_move_iterator(std::begin(n.assembled())),
                          std::make_move_iterator(std::end(n.assembled())));
    }
  }
}
#endif
} // namespace Lexer::Token