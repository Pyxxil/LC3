#include "Trap.hpp"

#include "Immediate.hpp"

namespace Lexer::Token {

Trap::Trap(std::string t, size_t t_line, size_t t_column,
           const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(1, {Match(TokenType::IMMEDIATE)})) {}

void Trap::assemble(uint16_t &program_counter, size_t width,
                    const std::map<std::string, Symbol> &symbols,
                    const std::string &sym) {
  const auto &ops = operands();

  const uint16_t trap_vector =
      static_cast<Immediate *>(ops.front().get())->value() & 0xFF;
  const uint16_t bin = 0xF000 | trap_vector;

  set_assembled(AssembledToken(
      bin,
      fmt::format(
          "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} TRAP x{5:X}",
          program_counter++, bin, line(), sym, width, trap_vector)));
}

} // namespace Lexer::Token