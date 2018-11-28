#ifndef TOKEN_STI_HPP
#define TOKEN_STI_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Sti : public Token {
public:
  explicit Sti(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::IMMEDIATE) |
                                   Match(TokenType::LABEL)})) {}

  Sti(const Sti &) = default;
  Sti(Sti &&) = default;

  Sti &operator=(const Sti &) = default;
  Sti &operator=(Sti &&) = default;

  TokenType tokenType() const final { return STI; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0xB000;

    uint16_t DR = static_cast<Register *>(ops[0].get())->reg() << 9;

    bin |= DR;

    if (TokenType::IMMEDIATE == ops[1]->tokenType()) {
      bin |= ((static_cast<int16_t>(
                   static_cast<Immediate *>(ops[1].get())->value() & 0x1FF)
               << 7) >>
              7);
    } else {
      auto label = std::find_if(symbols.begin(), symbols.end(),
                                [&token = ops[1]->getToken()](const auto &sym) {
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
                ops[1]->column(), ops[1]->getToken().length(), ""),
            fmt::format("Undefined label '{}'", *(ops[1])), ops[1]->file(),
            ops[1]->line());
        return;
      }
    }

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    setAssembled(AssembledToken(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} STI R{5:d} "
            "{6:s}",
            programCounter++, bin, line(),
            sym == symbols.end() ? "" : sym->second.name(), width, DR >> 9,
            TokenType::IMMEDIATE == ops[1]->tokenType()
                ? fmt::format("#{:d}",
                              static_cast<Immediate *>(ops[1].get())->value())
                : ops[1]->getToken())));
  }

  word memoryRequired() const override { return 1_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif