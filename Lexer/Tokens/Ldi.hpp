#ifndef TOKEN_LDI_HPP
#define TOKEN_LDI_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Ldi : public Token {
public:
  explicit Ldi(const std::string &t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::LABEL) |
                                   Match(TokenType::IMMEDIATE)})) {}

  Ldi(const Ldi &) = default;
  Ldi(Ldi &&) = default;

  Ldi &operator=(const Ldi &) = default;
  Ldi &operator=(Ldi &&) = default;

  TokenType token_type() const final { return LDI; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0xA000;

    uint16_t DR = static_cast<Register *>(ops[0].get())->reg() << 9;

    bin |= DR;

    if (TokenType::IMMEDIATE == ops[1]->token_type()) {
      bin |= ((static_cast<int16_t>(
                   static_cast<Immediate *>(ops[1].get())->value() & 0x1FF)
               << 7) >>
              7);
    } else {
      auto label =
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

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    set_assembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} LDI R{5:d} "
            "{6:s}",
            programCounter++, bin, line(),
            sym == symbols.end() ? "" : sym->second.name(), width, DR >> 9,
            TokenType::IMMEDIATE == ops[1]->token_type()
                ? fmt::format("#{:d}",
                              static_cast<Immediate *>(ops[1].get())->value())
                : ops[1]->get_token())));
  }

  word memory_required() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif