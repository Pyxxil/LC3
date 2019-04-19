#include "Jmpt.hpp"

#include "Register.hpp"

namespace Lexer::Token {

Jmpt::Jmpt(std::string t, size_t t_line, size_t t_column,
           const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(1, {Match(TokenType::REGISTER)})) {}

void Jmpt::assemble(uint16_t &program_counter, size_t width,
                    const std::map<std::string, Symbol> &symbols,
                    const std::string &sym) {

  const uint16_t reg = static_cast<Register *>(operands().front().get())->reg();
  const auto bin = static_cast<uint16_t>(0xC001 | (reg << 6));

  set_assembled(AssembledToken(
      bin, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                       "JMPT R{5:d}",
                       program_counter++, bin, line(), sym, width, reg)));
}

} // namespace Lexer::Token