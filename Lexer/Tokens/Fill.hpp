#ifndef TOKEN_FILL_HPP
#define TOKEN_FILL_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Fill : public Token {
public:
  explicit Fill(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file,
              Requirements(1, {Match(TokenType::IMMEDIATE) |
                               Match(TokenType::LABEL)})) {}

  Fill(const Fill &) = default;
  Fill(Fill &&) = default;

  Fill &operator=(const Fill &) = default;
  Fill &operator=(Fill &&) = default;

  TokenType token_type() const final { return FILL; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0x0000;

    auto label =
        std::find_if(symbols.begin(), symbols.end(),
                     [&token = ops.front()->get_token()](const auto &sym) {
                       return sym.second.name() == token;
                     });

    if (TokenType::IMMEDIATE == ops.front()->token_type()) {
      bin = static_cast<uint16_t>(static_cast<Immediate *>(ops.front().get())->value());
    } else if (label != symbols.end()) {
      bin = label->second.address();
    } else {
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(
              ops.front()->column(), ops.front()->get_token().length(), ""),
          fmt::format("Undefined label '{}'", *(ops.front())),
          ops.front()->file(), ops.front()->line());
      return;
    }

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
            ".FILL {5:s}",
            program_counter++, bin, line(),
            sym == symbols.end() ? "" : sym->second.name(), width,
            fmt::format(
                "0x{:04X}",
                TokenType::IMMEDIATE == ops.front()->token_type()
                    ? (static_cast<Immediate *>(ops.front().get())->value() &
                       0xFFFF)
                    : label->second.address()))));
  }

  word memory_required() const override { return 1_word; }

private:
}; // namespace Token
} // namespace Token
} // namespace Lexer

#endif