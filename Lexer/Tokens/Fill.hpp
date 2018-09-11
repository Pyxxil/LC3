#ifndef TOKEN_FILL_HPP
#define TOKEN_FILL_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Fill : public Token {
public:
  explicit Fill(std::string t, size_t tLine, size_t tColumn,
                const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(TokenType::IMMEDIATE) |
                               Match(TokenType::LABEL)})) {}

  Fill(const Fill &) = default;
  Fill(Fill &&) noexcept = default;

  Fill &operator=(const Fill &) = default;
  Fill &operator=(Fill &&) noexcept = default;

  TokenType tokenType() const final { return FILL; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0x0000;

    auto label =
        std::find_if(symbols.begin(), symbols.end(),
                     [&token = ops.front()->getToken()](const auto &sym) {
                       return sym.second.name() == token;
                     });

    if (TokenType::IMMEDIATE == ops.front()->tokenType()) {
      bin = static_cast<Immediate *>(ops.front().get())->value();
    } else if (label != symbols.end()) {
      bin = label->second.address();
    }

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    setAssembled(AssembledToken(
        bin, fmt::format(
                 "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                 ".FILL {5:s}",
                 programCounter++, bin, line(),
                 sym == symbols.end() ? "" : sym->second.name(), width,
                 fmt::format(
                     "0x{:04X}",
                     TokenType::IMMEDIATE == ops.front()->tokenType()
                         ? static_cast<Immediate *>(ops.front().get())->value()
                         : label->second.address()))));
  }

  word memoryRequired() const override { return 1_word; }

private:
}; // namespace Token
} // namespace Token
} // namespace Lexer

#endif