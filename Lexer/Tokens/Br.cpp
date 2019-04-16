#include "Br.hpp"

#include "Immediate.hpp"

namespace Lexer::Token {

Br::Br(std::string t, bool n, bool z, bool p, size_t t_line, size_t t_column,
       const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                1, {Match(TokenType::LABEL) | Match(TokenType::IMMEDIATE)})),
      N(n), Z(z), P(p) {}

void Br::assemble(int16_t &program_counter, size_t width,
                  const std::map<std::string, Symbol> &symbols,
                  const std::string &sym) {
  const auto &ops = operands();

  uint16_t bin = static_cast<uint16_t>(
      0x0000 | (static_cast<uint16_t>(N) << 11) |
      (static_cast<uint16_t>(Z) << 10) | (static_cast<uint16_t>(P) << 9));

  if (TokenType::IMMEDIATE == ops.front()->token_type()) {
    bin |= ((static_cast<int16_t>(
                 static_cast<Immediate *>(ops.front().get())->value() & 0x1FF)
             << 7) >>
            7) &
           0x1FF;
  } else {
    auto label =
        std::find_if(symbols.begin(), symbols.end(),
                     [&token = ops.front()->get_token()](const auto &sym) {
                       return sym.second.name() == token;
                     });
    if (label != symbols.end()) {
      bin |=
          (static_cast<int16_t>(static_cast<int16_t>(label->second.address() -
                                                     (program_counter + 1))
                                << 7) >>
           7) &
          0x1FF;
    } else {
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(
              ops[1]->column(), ops[1]->get_token().length(), ""),
          fmt::format("Undefined label '{}'", *(ops[1])), ops[1]->file(),
          ops[1]->line());
      return;
    }
  }

  set_assembled(AssembledToken(
      bin, fmt::format(
               "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
               "BR{5:s}{6:s}{7:s} {8:s}",
               program_counter++, bin, line(), sym, width, N ? "n" : "",
               Z ? "z" : "", P ? "p" : "",
               TokenType::IMMEDIATE == ops.front()->token_type()
                   ? fmt::format(
                         "#{:d}",
                         static_cast<Immediate *>(ops.front().get())->value())
                   : ops.front()->get_token())));
}

} // namespace Lexer::Token