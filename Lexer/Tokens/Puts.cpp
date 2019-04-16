#include "Puts.hpp"

namespace Lexer::Token {

Puts::Puts(std::string t, size_t t_line, size_t t_column,
           const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file) {}

void Puts::assemble(int16_t &program_counter, size_t width,
                    const std::map<std::string, Symbol> &symbols,
                    const std::string &sym) {

  set_assembled(AssembledToken(
      0xF022,
      fmt::format("({0:0>4X}) F022 1111000000100010 ({1: >4d}) {2: <{3}s} PUTS",
                  program_counter++, line(), sym, width)));
}

} // namespace Lexer::Token