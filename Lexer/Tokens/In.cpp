#include "In.hpp"

namespace Lexer::Token {

In::In(std::string t, size_t t_line, size_t t_column, const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file) {}

void In::assemble(int16_t &program_counter, size_t width,
                  const std::map<std::string, Symbol> &symbols,
                  const std::string &sym) {
  set_assembled(AssembledToken(
      0xF023,
      fmt::format("({0:0>4X}) F023 1111000000100011 ({1: >4d}) {2: <{3}s} IN",
                  program_counter++, line(), sym, width)));
}

} // namespace Lexer::Token