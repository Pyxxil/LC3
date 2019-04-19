#include "Neg.hpp"

#include "Register.hpp"

namespace Lexer::Token {
#ifdef ADDONS
Neg::Neg(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                1, {Match(TokenType::REGISTER), Match(TokenType::REGISTER)},
                2)) {}

void Neg::assemble(uint16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  const uint16_t DR = static_cast<Register *>(ops.front().get())->reg();
  const uint16_t SR =
      ops.size() == 2 ? static_cast<Register *>(ops[1].get())->reg() : DR;
  const uint16_t bin =
      static_cast<const uint16_t>(0x903F | (DR << 9) | (SR << 6));

  set_assembled(AssembledToken(
      bin, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: "
                       "<{4}s} NOT R{5:d} "
                       "R{6:d}",
                       program_counter++, bin, line(), sym, width, DR, SR)));

  const uint16_t bin1 =
      static_cast<const uint16_t>(0x1021 | (DR << 9) | (SR << 6));

  as_assembled.emplace_back(
      bin1,
      fmt::format(
          "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} ADD R{5:d} "
          "R{6:d} #1",
          program_counter++, bin1, line(), std::string{}, width, DR, SR));
}
#endif
} // namespace Lexer::Token