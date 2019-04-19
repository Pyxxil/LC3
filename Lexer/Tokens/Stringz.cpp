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

  const auto &firstString =
      static_cast<String *>(ops.front().get())->true_token();
  const auto len = firstString.length();

  set_assembled(AssembledToken(
      static_cast<int16_t>(firstString.front()),
      fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                  ".FILL 0x{5:04X}",
                  program_counter++, static_cast<int16_t>(firstString.front()),
                  line(), sym, width,
                  static_cast<int16_t>(firstString.front()))));

  for (auto idx = 1; idx < len; ++idx) {
    as_assembled.emplace_back(
        static_cast<uint16_t>(firstString[idx]),
        fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                    ".FILL 0x{5:04X}",
                    program_counter++, static_cast<int16_t>(firstString[idx]),
                    line(), std::string{}, width,
                    static_cast<int16_t>(firstString[idx])));
  }

  as_assembled.emplace_back(
      0, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                     ".FILL 0x0000",
                     program_counter++, 0, line(), std::string{}, width));

  for (auto idx = 1; idx < ops.size(); ++idx) {
    for (auto chr : static_cast<String *>(ops[idx].get())->true_token()) {
      as_assembled.emplace_back(
          static_cast<uint16_t>(chr),
          fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                      ".FILL 0x{5:04X}",
                      program_counter++, static_cast<int16_t>(chr), line(),
                      std::string{}, width, static_cast<int16_t>(chr)));
    }
    as_assembled.emplace_back(
        0, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                       ".FILL 0x0000",
                       program_counter++, 0, line(), std::string{}, width));
  }
}

} // namespace Lexer::Token