#include "Lshift.hpp"

#include "Register.hpp"

namespace Lexer::Token {
#ifdef ADDONS
Lshift::Lshift(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(2, {Match(TokenType::REGISTER),
                             Match(TokenType::IMMEDIATE)})) {}

void Lshift::assemble(int16_t &programCounter, size_t width,
                      const std::map<std::string, Symbol> &symbols,
                      const std::string &sym) {
  const auto &ops = operands();

  const uint16_t reg = static_cast<Register *>(ops.front().get())->reg();
  const uint16_t bin = 0x1000 | (reg << 9) | (reg << 6) | reg;
  const uint16_t count = static_cast<Immediate *>(ops[1].get())->value();

  for (auto i = 0; i < count; i++) {
    as_assembled.emplace_back(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} ADD R{5:d} "
            "R{5:d} R{5:d}",
            programCounter++, bin, line(), sym, width, reg));
  }
}
#endif
} // namespace Lexer::Token