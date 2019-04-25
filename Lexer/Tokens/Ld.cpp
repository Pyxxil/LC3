#include "Ld.hpp"

#include "Immediate.hpp"
#include "Register.hpp"

namespace Lexer::Token {

Ld::Ld(std::string t, size_t t_line, size_t t_column, const std::string &t_file)
    : Token(std::move(t), t_line, t_column, t_file,
            Requirements(
                2, {Match(TokenType::REGISTER),
                    Match(TokenType::LABEL) | Match(TokenType::IMMEDIATE)})) {}

void Ld::assemble(uint16_t &program_counter, size_t width,
                  const std::map<std::string, Symbol> &symbols,
                  const std::string &sym) {
  const auto &ops = operands();

  const auto DR = static_cast<Register *>(ops.front().get())->reg();
  int16_t offset;

  const bool is_immediate = TokenType::IMMEDIATE == ops.back()->token_type();

  if (is_immediate) {
    offset = static_cast<Immediate *>(ops.back().get())->value();
  } else {
    const auto label =
        std::find_if(symbols.begin(), symbols.end(),
                     [&token = ops.back()->get_token()](auto &&sym) {
                       return sym.second.name() == token;
                     });

    if (label == symbols.end()) {
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(
              ops.back()->column(), ops.back()->get_token().length(), ""),
          fmt::format("Undefined label '{}'", *(ops.back())),
          ops.back()->file(), ops.back()->line());
      return;
    }

    offset =
        static_cast<int16_t>(label->second.address()) - (program_counter + 1);
  }

  const auto bin =
      static_cast<uint16_t>(OP_LD | DR << 9 | mask<9>(sign_extend<7>(offset)));

  set_assembled(AssembledToken(
      bin, fmt::format(
               "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} LD R{5:d} "
               "{6:s}",
               program_counter++, bin, line(), sym, width, DR,
               is_immediate ? fmt::format("#{:d}", offset)
                            : ops.back()->get_token())));
}

} // namespace Lexer::Token