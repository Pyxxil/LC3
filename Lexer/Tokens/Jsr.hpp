#ifndef TOKEN_JSR_HPP
#define TOKEN_JSR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Jsr : public Token {
public:
  explicit Jsr(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file)
      : Token(std::move(t), t_line, t_column,t_file,
              Requirements(1, {Match(TokenType::LABEL) |
                               Match(TokenType::IMMEDIATE)})) {}

  Jsr(const Jsr &) = default;
  Jsr(Jsr &&) = default;

  Jsr &operator=(const Jsr &) = default;
  Jsr &operator=(Jsr &&) = default;

  TokenType token_type() const final { return JSR; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0x4800;

    if (TokenType::IMMEDIATE == ops.front()->token_type()) {
      bin |= ((static_cast<int16_t>(
                   static_cast<Immediate *>(ops.front().get())->value() & 0x7FF)
               << 5) >>
              5) &
             0x7FF;
    } else {
      auto label =
          std::find_if(symbols.begin(), symbols.end(),
                       [&token = ops.front()->get_token()](const auto &sym) {
                         return sym.second.name() == token;
                       });
      if (label != symbols.end()) {
        bin |= ((static_cast<int16_t>(label->second.address() -
                                      (program_counter + 1))
                 << 5) >>
                5) &
               0x7FF;
      } else {
        Notification::error_notifications << Diagnostics::Diagnostic(
            std::make_unique<Diagnostics::DiagnosticHighlighter>(
                ops.front()->column(), ops.front()->get_token().length(), ""),
            fmt::format("Undefined label '{}'", *(ops.front())),
            ops.front()->file(), ops.front()->line());
        return;
      }
    }

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [program_counter](const auto &sym) {
                              return sym.second.address() == program_counter;
                            });

    set_assembled(AssembledToken(
        bin, fmt::format(
                 "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                 "JSR {5:s}",
                 program_counter++, bin, line(),
                 sym == symbols.end() ? "" : sym->second.name(), width,
                 TokenType::IMMEDIATE == ops.front()->token_type()
                     ? fmt::format(
                           "#{:d}",
                           static_cast<Immediate *>(ops.front().get())->value())
                     : ops.front()->get_token())));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif