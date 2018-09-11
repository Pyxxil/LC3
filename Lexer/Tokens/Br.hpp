#ifndef TOKEN_BR_HPP
#define TOKEN_BR_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Br : public Token {
public:
  Br(std::string t, bool n, bool z, bool p, size_t tLine, size_t tColumn,
     const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(
                  1, {Match(TokenType::LABEL) | Match(TokenType::IMMEDIATE)})),
        N(n), Z(z), P(p) {}

  Br(const Br &) = default;
  Br(Br &&) noexcept = default;

  Br &operator=(const Br &) = default;
  Br &operator=(Br &&) noexcept = default;

  TokenType tokenType() const final { return BR; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0x0000 | (static_cast<uint16_t>(N) << 11) |
                   (static_cast<uint16_t>(Z) << 10) |
                   (static_cast<uint16_t>(P) << 9);

    if (TokenType::IMMEDIATE == ops.front()->tokenType()) {
      bin |= ((static_cast<int16_t>(
                   static_cast<Immediate *>(ops.front().get())->value() & 0x1FF)
               << 7) >>
              7) &
             0x1FF;
    } else {
      auto label =
          std::find_if(symbols.begin(), symbols.end(),
                       [&token = ops.front()->getToken()](const auto &sym) {
                         return sym.second.name() == token;
                       });
      if (label != symbols.end()) {
        bin |=
            (static_cast<int16_t>(static_cast<int16_t>(label->second.address() -
                                                       (programCounter + 1))
                                  << 7) >>
             7) &
            0x1FF;
      }
    }

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    setAssembled(AssembledToken(
        bin, fmt::format(
                 "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                 "BR{5:s}{6:s}{7:s} {8:s}",
                 programCounter++, bin, line(),
                 sym == symbols.end() ? "" : sym->second.name(), width,
                 N ? "n" : "", Z ? "z" : "", P ? "p" : "",
                 TokenType::IMMEDIATE == ops.front()->tokenType()
                     ? fmt::format(
                           "#{:d}",
                           static_cast<Immediate *>(ops.front().get())->value())
                     : ops.front()->getToken())));
  }

  word memoryRequired() const override { return 1_word; }

private:
  bool N;
  bool Z;
  bool P;
};
} // namespace Token
} // namespace Lexer

#endif