#include "Halt.hpp"

namespace Lexer::Token {

Halt::Halt(std::string t, size_t t_line, size_t t_column,
           const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file) {}

void Halt::assemble(int16_t &program_counter, size_t width,
                    const std::map<std::string, Symbol> &symbols,
                    const std::string &sym) {
  set_assembled(AssembledToken(
      0xF025,
      fmt::format("({0:0>4X}) F025 1111000000100101 ({1: >4d}) {2: <{3}s} HALT",
                  program_counter++, line(), sym, width)));
}

} // namespace Lexer::Token
