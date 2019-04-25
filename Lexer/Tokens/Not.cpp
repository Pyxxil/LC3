#include "Not.hpp"

#include "Register.hpp"

#include "TokenExtras.hpp"

namespace Lexer::Token {

Not::Not(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                1, {Match(TokenType::REGISTER), Match(TokenType::REGISTER)},
                2)) {}

void Not::assemble(uint16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  const auto DR = static_cast<Register *>(ops.front().get())->reg();
  const auto SR1 = static_cast<Register *>(ops.back().get())->reg();
  const auto bin = static_cast<uint16_t>(OP_NOT | DR << 9 | SR1 << 6);

  set_assembled(AssembledToken(
      bin, fmt::format(
               "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} NOT R{5:d} "
               "R{6:d}",
               program_counter++, bin, line(), sym, width, DR, SR1)));
}

} // namespace Lexer::Token