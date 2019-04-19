#include "And.hpp"

#include "Immediate.hpp"
#include "Register.hpp"

namespace Lexer::Token {

And::And(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                2,
                {Match(TokenType::REGISTER), Match(TokenType::REGISTER),
                 Match(TokenType::REGISTER) | Match(TokenType::IMMEDIATE)},
                3)) {}

void And::assemble(uint16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  const uint16_t DR = static_cast<Register *>(ops[0].get())->reg();
  const uint16_t SR1 =
      ops.size() == 2 ? DR : static_cast<Register *>(ops[1].get())->reg();
  const uint16_t SR2 =
      ops.size() == 2
          ? static_cast<Register *>(ops[1].get())->reg()
          : (TokenType::REGISTER == ops[2]->token_type()
                 ? static_cast<Register *>(ops[2].get())->reg()
                 : (0x20 |
                    (static_cast<Immediate *>(ops[2].get())->value() & 0x1F)));

  const auto bin = static_cast<uint16_t>(0x5000 | (DR << 9) | (SR1 << 6) | SR2);

  set_assembled(AssembledToken(
      bin, fmt::format(
               "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} AND R{5:d} "
               "R{6:d} {7:s}",
               program_counter++, bin, line(), sym, width, DR, SR1,
               fmt::format("{}{:d}", bin & 0x20 ? '#' : 'R',
                           (static_cast<int16_t>(SR2) << 11) >> 11))));
}

} // namespace Lexer::Token