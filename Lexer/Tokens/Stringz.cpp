#include "Stringz.hpp"

namespace Lexer::Token {

Stringz::Stringz(std::string t, size_t t_line, size_t t_column,
                 const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(1, {Match(STRING)}, static_cast<size_t>(-1u))) {}

void Stringz::assemble(uint16_t &program_counter, size_t width,
                       const std::map<std::string, Symbol> &symbols,
                       const std::string &sym) {
  const auto &ops = operands();

  const auto &first_string =
      static_cast<String *>(ops.front().get())->true_token();
  const auto len = first_string.length();

  set_assembled(AssembledToken(
      static_cast<int16_t>(first_string.front()),
      fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                  ".FILL 0x{1:04X}",
                  program_counter++, static_cast<int16_t>(first_string.front()),
                  line(), sym, width)));

  const auto blank = std::string(width, ' ');

  for (auto idx = 1; idx < len; ++idx) {
    as_assembled.emplace_back(
        static_cast<uint16_t>(first_string[idx]),
        fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3:s} "
                    ".FILL 0x{1:04X}",
                    program_counter++, static_cast<int16_t>(first_string[idx]),
                    line(), blank));
  }

  as_assembled.emplace_back(
      0, fmt::format("({0:0>4X}) 0000 0000000000000000 ({1: >4d}) {2:s} "
                     ".FILL 0x0000",
                     program_counter++, line(), blank));

  for (auto idx = 1; idx < ops.size(); ++idx) {
    for (auto chr : static_cast<String *>(ops[idx].get())->true_token()) {
      as_assembled.emplace_back(
          static_cast<uint16_t>(chr),
          fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3:s} "
                      ".FILL 0x{1:04X}",
                      program_counter++, static_cast<int16_t>(chr), line(),
                      blank));
    }
    as_assembled.emplace_back(
        0, fmt::format("({0:0>4X}) 0000 0000000000000000 ({1: >4d}) {2s} "
                       ".FILL 0x0000",
                       program_counter++, line(), blank));
  }
}

} // namespace Lexer::Token