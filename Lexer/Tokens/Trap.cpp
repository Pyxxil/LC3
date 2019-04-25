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

  const auto trap_vector =
      mask<8>(static_cast<Immediate *>(ops.front().get())->value());
  const auto bin = static_cast<uint16_t>(OP_TRAP | trap_vector);

  set_assembled(AssembledToken(
      bin,
      fmt::format(
          "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} TRAP x{5:X}",
          program_counter++, bin, line(), sym, width, trap_vector)));
}

} // namespace Lexer::Token