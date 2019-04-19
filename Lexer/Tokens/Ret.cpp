#include "Ret.hpp"

namespace Lexer::Token {

Ret::Ret(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file, Requirements()) {}

void Ret::assemble(uint16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {

  set_assembled(AssembledToken(
      0xC1C0,
      fmt::format("({0:0>4X}) 1100000111000000 C1C0 ({1: >4d}) {2: <{3}s} "
                  "RET",
                  program_counter++, line(), sym, width)));
}

} // namespace Lexer::Token