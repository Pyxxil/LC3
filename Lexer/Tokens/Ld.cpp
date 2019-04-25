#include "Ld.hpp"

#include "Immediate.hpp"
#include "Register.hpp"

namespace Lexer::Token {

Ld::Ld(std::string t, size_t t_line, size_t t_column, const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                2, {Match(TokenType::REGISTER),
                    Match(TokenType::LABEL) | Match(TokenType::IMMEDIATE)})) {}

void Ld::assemble(uint16_t &program_counter, size_t width,
                  const std::map<std::string, Symbol> &symbols,
                  const std::string &sym) {
  const auto &ops = operands();

  const auto DR = static_cast<Register *>(ops.front().get())->reg();
  int16_t offset;

  const bool is_immediate = TokenType::IMMEDIATE == ops.back()->token_type();
  if (is_immediate) {
    offset = static_cast<Immediate *>(ops.back().get())->value();
  } else {
    if (const auto label =
            find_symbol(symbols, ops.back()->get_token(), *(ops.back().get()));
        label == symbols.cend()) {
      return;
    } else {
      offset =
          static_cast<int16_t>(label->second.address()) - (program_counter + 1);
    }
  }

  const auto bin =
      static_cast<uint16_t>(OP_LD | DR << 9 | mask<9>(sign_extend<7>(offset)));

  set_assembled(AssembledToken(
      bin, fmt::format(
               "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} LD R{5:d} "
               "{6:s}",
               program_counter++, bin, line(), sym, width, DR,
               is_immediate ? fmt::format("#{:d}", offset)
                            : ops.back()->get_token())));
}

} // namespace Lexer::Token