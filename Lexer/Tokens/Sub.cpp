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

void Sub::assemble(int16_t &programCounter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  const size_t first_reg_idx = ops.size() - 2;
  const size_t second_reg_idx = ops.size() - 1;

  if (static_cast<Register *>(ops[first_reg_idx].get())->reg() ==
      static_cast<Register *>(ops[second_reg_idx].get())->reg()) {
    And a("AND", line(), column(), file());
    a.add_operand(std::make_unique<Register>(
        fmt::format("R{:d}", static_cast<Register *>(ops.front().get())->reg()),
        line(), column(), file()));
    a.add_operand(std::make_unique<Register>(
        fmt::format("R{:d}",
                    static_cast<Register *>(ops[first_reg_idx].get())->reg()),
        line(), column(), file()));
    a.add_operand(std::make_unique<Decimal>(
        fmt::format("0",
                    static_cast<Register *>(ops[second_reg_idx].get())->reg()),
        line(), column(), file()));
    a.assemble(programCounter, width, symbols, sym);

    as_assembled = a.assembled();
  } else {
    const uint16_t DR = static_cast<Register *>(ops.front().get())->reg();
    const uint16_t SR1 =
        static_cast<Register *>(ops[first_reg_idx].get())->reg();
    const uint16_t SR2 =
        static_cast<Register *>(ops[second_reg_idx].get())->reg();

    Neg n(".NEG", line(), column(), file());
    n.add_operand(std::make_unique<Register>(fmt::format("R{:d}", SR2), line(),
                                             column(), file()));
    n.assemble(programCounter, width, symbols, sym);

    Add a("ADD", line(), column(), file());
    a.add_operand(std::make_unique<Register>(fmt::format("R{:d}", DR), line(),
                                             column(), file()));
    a.add_operand(std::make_unique<Register>(fmt::format("R{:d}", SR1), line(),
                                             column(), file()));
    a.add_operand(std::make_unique<Register>(fmt::format("R{:d}", SR2), line(),
                                             column(), file()));
    a.assemble(programCounter, width, symbols, std::string{});

    as_assembled = n.assembled();

    as_assembled.insert(std::end(as_assembled),
                        std::make_move_iterator(a.assembled().begin()),
                        std::make_move_iterator(a.assembled().end()));

    if (DR != SR2) {
      Neg n(".NEG", line(), column(), file());
      n.add_operand(std::make_unique<Register>(fmt::format("R{:d}", SR2),
                                               line(), column(), file()));
      n.assemble(programCounter, width, symbols, std::string{});

      as_assembled.insert(std::end(as_assembled),
                          std::make_move_iterator(n.assembled().begin()),
                          std::make_move_iterator(n.assembled().end()));
    }
  }
}
#endif
} // namespace Lexer::Token