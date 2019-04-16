#include "Trap.hpp"

#include "Immediate.hpp"

namespace Lexer::Token {

Trap::Trap(std::string t, size_t t_line, size_t t_column,
           const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(1, {Match(TokenType::IMMEDIATE)})) {}

void Trap::assemble(int16_t &programCounter, size_t width,
                    const std::map<std::string, Symbol> &symbols,
                    const std::string &sym) {
  const auto &ops = operands();

  const uint16_t bin =
      0xF000 | (static_cast<Immediate *>(ops.front().get())->value() & 0xFF);

  set_assembled(AssembledToken(
      bin,
      fmt::format(
          "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} TRAP x{5:X}",
          programCounter++, bin, line(), sym, width,
          static_cast<Immediate *>(ops.front().get())->value() & 0xFF)));
}

} // namespace Lexer::Token