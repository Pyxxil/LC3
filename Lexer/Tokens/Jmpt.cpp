#include "Jmpt.hpp"

#include "Register.hpp"

namespace Lexer::Token {

Jmpt::Jmpt(std::string t, size_t t_line, size_t t_column,
           const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(1, {Match(TokenType::REGISTER)})) {}

void Jmpt::assemble(int16_t &programCounter, size_t width,
                    const std::map<std::string, Symbol> &symbols,
                    const std::string &sym) {
  const uint16_t reg = static_cast<Register *>(operands().front().get())->reg()
                       << 6;
  const auto bin = 0xC001 | reg;

  set_assembled(AssembledToken(
      bin, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                       "JMPT R{5:d}",
                       programCounter++, bin, line(), sym, width, reg >> 6)));
}

} // namespace Lexer::Token