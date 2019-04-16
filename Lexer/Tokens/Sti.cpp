#include "Sti.hpp"

#include "Immediate.hpp"
#include "Register.hpp"

namespace Lexer::Token {

Sti::Sti(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                2, {Match(TokenType::REGISTER),
                    Match(TokenType::IMMEDIATE) | Match(TokenType::LABEL)})) {}

void Sti::assemble(int16_t &programCounter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  const uint16_t DR = static_cast<Register *>(ops[0].get())->reg() << 9;

  uint16_t bin = 0xB000 | DR;

  if (TokenType::IMMEDIATE == ops[1]->token_type()) {
    bin |= ((static_cast<int16_t>(
                 static_cast<Immediate *>(ops[1].get())->value() & 0x1FF)
             << 7) >>
            7);
  } else {
    const auto label =
        std::find_if(symbols.begin(), symbols.end(),
                     [&token = ops[1]->get_token()](const auto &sym) {
                       return sym.second.name() == token;
                     });
    if (label != symbols.end()) {
      bin |=
          (static_cast<int16_t>(static_cast<int16_t>(label->second.address() -
                                                     (programCounter + 1))
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
      bin,
      fmt::format(
          "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} STI R{5:d} "
          "{6:s}",
          programCounter++, bin, line(), sym, width, DR >> 9,
          TokenType::IMMEDIATE == ops[1]->token_type()
              ? fmt::format("#{:d}",
                            static_cast<Immediate *>(ops[1].get())->value())
              : ops[1]->get_token())));
}
} // namespace Lexer::Token