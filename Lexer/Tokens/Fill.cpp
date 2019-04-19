#include "Fill.hpp"

#include "Immediate.hpp"

namespace Lexer::Token {

Fill::Fill(std::string t, size_t t_line, size_t t_column,
           const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                1, {Match(TokenType::IMMEDIATE) | Match(TokenType::LABEL)})) {}

void Fill::assemble(uint16_t &program_counter, size_t width,
                    const std::map<std::string, Symbol> &symbols,
                    const std::string &sym) {
  const auto &ops = operands();

  uint16_t bin;

  if (TokenType::IMMEDIATE == ops.front()->token_type()) {
    bin = static_cast<Immediate *>(ops.front().get())->value();
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

    bin = label->second.address();
  }

  set_assembled(AssembledToken(
      bin, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                       ".FILL 0x{5:04x}",
                       program_counter++, bin, line(), sym, width, bin)));
}

} // namespace Lexer::Token