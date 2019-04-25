#include "Add.hpp"

#include "Immediate.hpp"
#include "Register.hpp"

namespace Lexer::Token {

Add::Add(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                2,
                {Match(TokenType::REGISTER), Match(TokenType::REGISTER),
                 Match(TokenType::REGISTER) | Match(TokenType::IMMEDIATE)},
                3)) {}

void Add::assemble(uint16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  const auto DR = static_cast<Register *>(ops.front().get())->reg();
  const auto SR1 =
      ops.size() == 2 ? DR : static_cast<Register *>(ops[1].get())->reg();
  const auto SR2 =
      (TokenType::REGISTER == ops.back()->token_type())
          ? static_cast<Register *>(ops.back().get())->reg()
          : (IMM5_TOGGLE |
             mask<5>(static_cast<Immediate *>(ops.back().get())->value()));

  const auto bin = static_cast<uint16_t>(OP_ADD | DR << 9 | SR1 << 6 | SR2);

  set_assembled(AssembledToken(
      bin, fmt::format(
               "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} ADD R{5:d} "
               "R{6:d} {7}{8:d}",
               program_counter++, bin, line(), sym, width, DR, SR1,
               bin & IMM5_TOGGLE ? '#' : 'R', sign_extend<11>(SR2))));
}

} // namespace Lexer::Token