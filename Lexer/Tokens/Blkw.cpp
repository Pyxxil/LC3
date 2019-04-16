#include "Blkw.hpp"

namespace Lexer::Token {

Blkw::Blkw(std::string t, size_t t_line, size_t t_column,
           const std::string &t_file)
    : Token(
          std::move(t), t_line, t_column, t_file,
          Requirements(1,
                       {Match(TokenType::IMMEDIATE),
                        Match(TokenType::IMMEDIATE) | Match(TokenType::LABEL)},
                       2)) {}

void Blkw::assemble(int16_t &program_counter, size_t width,
                    const std::map<std::string, Symbol> &symbols,
                    const std::string &sym) {
  const auto &ops = operands();

  uint16_t bin = 0x0000;

  std::map<std::string, Symbol>::const_iterator label;

  if (ops.size() > 1) {
    if (TokenType::IMMEDIATE == ops[1]->token_type()) {
      bin = static_cast<uint16_t>(
          static_cast<Immediate *>(ops[1].get())->value());
    } else {
      label = std::find_if(symbols.begin(), symbols.end(),
                           [&token = ops[1]->get_token()](const auto &sym) {
                             return sym.second.name() == token;
                           });
      if (label != symbols.end()) {
        bin = label->second.address();
      } else {
        Notification::error_notifications << Diagnostics::Diagnostic(
            std::make_unique<Diagnostics::DiagnosticHighlighter>(
                ops[1]->column(), ops[1]->get_token().length(), ""),
            fmt::format("Undefined label '{}'", *ops[1]), ops[1]->file(),
            ops[1]->line());
        return;
      }
    }
  }

  const auto value =
      ops.size() == 1
          ? "0x0000"
          : fmt::format(
                "0x{:04X}",
                TokenType::IMMEDIATE == ops[1]->token_type()
                    ? (static_cast<Immediate *>(ops[1].get())->value() & 0xFFFF)
                    : label->second.address());

  set_assembled(AssembledToken(
      bin, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                       ".FILL {5:s}",
                       program_counter++, bin, line(), sym, width, value)));

  for (auto i = static_cast<Immediate *>(ops.front().get())->value() - 1; i > 0;
       i--) {
    as_assembled.emplace_back(
        bin, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                         ".FILL {5:s}",
                         program_counter++, bin, line(), std::string{}, width,
                         value));
  }
}
} // namespace Lexer::Token