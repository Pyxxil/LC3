#include "Not.hpp"

#include "Register.hpp"

namespace Lexer::Token {

Not::Not(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                1, {Match(TokenType::REGISTER), Match(TokenType::REGISTER)},
                2)) {}

void Not::assemble(int16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  const uint16_t DR = static_cast<Register *>(ops[0].get())->reg() << 9;
  const uint16_t SR1 = ops.size() > 1
                           ? static_cast<Register *>(ops[1].get())->reg() << 6
                           : DR >> 3;
  const uint16_t bin = 0x903F | DR | SR1;

  set_assembled(AssembledToken(
      bin, fmt::format(
               "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} NOT R{5:d} "
               "R{6:d}",
               program_counter++, bin, line(), sym, width, DR >> 9, SR1 >> 6)));
}

} // namespace Lexer::Token