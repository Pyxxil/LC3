#include "Getc.hpp"

namespace Lexer::Token {

Getc::Getc(std::string t, size_t t_line, size_t t_column,
           const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file, Requirements()) {}

void Getc::assemble(uint16_t &program_counter, size_t width,
                    const std::map<std::string, Symbol> &symbols,
                    const std::string &sym) {
  set_assembled(AssembledToken(
      0xF020,
      fmt::format("({0:0>4X}) F020 1111000000100000 ({1: >4d}) {2: <{3}s} GETC",
                  program_counter++, line(), sym, width)));
}

} // namespace Lexer::Token
