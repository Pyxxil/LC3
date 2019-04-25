#include "Ldr.hpp"

#include "Immediate.hpp"
#include "Register.hpp"

namespace Lexer::Token {

Ldr::Ldr(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(3, {Match(TokenType::REGISTER),
                             Match(TokenType::REGISTER),
                             Match(TokenType::IMMEDIATE)})) {}

void Ldr::assemble(uint16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  const auto DR = static_cast<Register *>(ops[0].get())->reg();
  const auto SR = static_cast<Register *>(ops[1].get())->reg();
  const auto offset =
      sign_extend<10>(static_cast<Immediate *>(ops[2].get())->value()) & 0x3F;

  const auto bin =
      static_cast<const uint16_t>(OP_LDR | DR << 9 | SR << 6 | offset);

  set_assembled(AssembledToken(
      bin, fmt::format(
               "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} LDR R{5:d} "
               "R{6:d} #{7:d}",
               program_counter++, bin, line(), sym, width, DR, SR, offset)));
}
} // namespace Lexer::Token