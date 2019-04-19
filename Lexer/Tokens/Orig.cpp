#include "Orig.hpp"

#include "Immediate.hpp"

namespace Lexer::Token {

Orig::Orig(std::string t, size_t t_line, size_t t_column,
           const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(1, {Match(TokenType::IMMEDIATE)})) {}

void Orig::assemble(uint16_t &program_counter, size_t width,
                    const std::map<std::string, Symbol> &symbols,
                    const std::string &sym) {
  program_counter = static_cast<Immediate *>(operands()[0].get())->value();

  set_assembled(AssembledToken(
      program_counter,
      fmt::format(
          "(0000) {0:0>4X} {0:0>16b} ({1: >4d}) {2: <{3}s} .ORIG 0x{0:0>4X}",
          program_counter, line(), std::string{}, width)));
}

} // namespace Lexer::Token