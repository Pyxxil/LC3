#include "Out.hpp"

namespace Lexer::Token {

Out::Out(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file) {}

void Out::assemble(int16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {

  set_assembled(AssembledToken(
      0xF021,
      fmt::format("({0:0>4X}) F021 1111000000100001 ({1: >4d}) {2: <{3}s} OUT",
                  program_counter++, line(), sym, width)));
}

} // namespace Lexer::Token