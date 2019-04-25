#include "Jsr.hpp"

#include "Immediate.hpp"

namespace Lexer::Token {

Jsr::Jsr(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                1, {Match(TokenType::LABEL) | Match(TokenType::IMMEDIATE)})) {}

void Jsr::assemble(uint16_t &program_counter, size_t width,
                   const std::map<std::string, Symbol> &symbols,
                   const std::string &sym) {
  const auto &ops = operands();

  int16_t offset = 0;

  if (TokenType::IMMEDIATE == ops.front()->token_type()) {
    offset = static_cast<Immediate *>(ops.front().get())->value() & 0x7FF;
  } else {
    const auto label =
        std::find_if(symbols.begin(), symbols.end(),
                     [&token = ops.front()->get_token()](auto &&sym) {
                       return sym.second.name() == token;
                     });

    if (label == symbols.end()) {
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(
              ops.front()->column(), ops.front()->get_token().length(), ""),
          fmt::format("Undefined label '{}'", *(ops.front())),
          ops.front()->file(), ops.front()->line());
      return;
    }

    offset =
        static_cast<int16_t>(label->second.address()) - (program_counter + 1);
  }

  const auto bin =
      static_cast<uint16_t>(OP_JSR | 0x0800 | (sign_extend<5>(offset) & 0x7FF));

  set_assembled(AssembledToken(
      bin, fmt::format(
               "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
               "JSR {5:s}",
               program_counter++, bin, line(), sym, width,
               TokenType::IMMEDIATE == ops.front()->token_type()
                   ? fmt::format(
                         "#{:d}",
                         static_cast<Immediate *>(ops.front().get())->value())
                   : ops.front()->get_token())));
}

} // namespace Lexer::Token