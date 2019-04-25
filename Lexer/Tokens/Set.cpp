#include "Set.hpp"

#include "Add.hpp"
#include "And.hpp"
#include "Br.hpp"
#include "Decimal.hpp"
#include "Fill.hpp"
#include "Ld.hpp"
#include "Register.hpp"

namespace Lexer::Token {
#ifdef ADDONS
Set::Set(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                2, {Match(TokenType::REGISTER),
                    Match(TokenType::IMMEDIATE) | Match(TokenType::LABEL)})) {}

void Set::assemble(uint16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  const int16_t value = static_cast<Immediate *>(ops[1].get())->value();
  const auto reg = static_cast<Register *>(ops.front().get())->reg();

  auto &&reg_string = fmt::format("R{:d}", reg);
  auto &&val_string = fmt::format("#{:d}", value);

  if (value > -16 && value < 15) {
    And a("AND", line(), column(), file());
    a.add_operand(
        std::make_unique<Register>(reg_string, line(), column(), file()));
    a.add_operand(
        std::make_unique<Register>(reg_string, line(), column(), file()));
    a.add_operand(std::make_unique<Immediate>("#0", line(), column(), file()));
    a.assemble(program_counter, width, symbols, sym);

    Add ad("ADD", line(), column(), file());
    ad.add_operand(
        std::make_unique<Register>(reg_string, line(), column(), file()));
    ad.add_operand(
        std::make_unique<Register>(reg_string, line(), column(), file()));
    ad.add_operand(
        std::make_unique<Decimal>(val_string, line(), column(), file()));
    ad.assemble(program_counter, width, symbols, std::string{});

    as_assembled = a.assembled();
    as_assembled.insert(std::end(as_assembled),
                        std::make_move_iterator(std::begin(ad.assembled())),
                        std::make_move_iterator(std::begin(ad.assembled())));
  } else {
    Br b("BR", true, true, true, line(), column(), file());
    b.add_operand(std::make_unique<Decimal>("#1", line(), column(), file()));
    b.assemble(program_counter, width, symbols, sym);

    Fill f(".FILL", line(), column(), file());
    f.add_operand(
        std::make_unique<Decimal>(val_string, line(), column(), file()));
    f.assemble(program_counter, width, symbols, std::string{});

    Ld l("LD", line(), column(), file());
    l.add_operand(
        std::make_unique<Register>(reg_string, line(), column(), file()));
    l.add_operand(std::make_unique<Decimal>("#-2", line(), column(), file()));
    l.assemble(program_counter, width, symbols, std::string{});

    as_assembled = b.assembled();
    as_assembled.insert(std::end(as_assembled),
                        std::make_move_iterator(std::begin(f.assembled())),
                        std::make_move_iterator(std::begin(f.assembled())));
    as_assembled.insert(std::end(as_assembled),
                        std::make_move_iterator(std::begin(l.assembled())),
                        std::make_move_iterator(std::begin(l.assembled())));
  }
}
#endif
} // namespace Lexer::Token