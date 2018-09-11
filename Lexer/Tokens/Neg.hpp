#ifndef TOKEN_NEG_HPP
#define TOKEN_NEG_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Neg : public Token {
public:
  Neg(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(
                  1, {Match(TokenType::REGISTER), Match(TokenType::REGISTER)},
                  2)) {}

  Neg(const Neg &) = default;
  Neg(Neg &&) noexcept = default;

  Neg &operator=(const Neg &) = default;
  Neg &operator=(Neg &&) noexcept = default;

  TokenType tokenType() const final { return NEG; }

  // TODO: This could also just alter the NOT instructions lower bits to make
  // the simulator do 2's complement for us..
  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    uint16_t bin = 0x903F;

    uint16_t DR = static_cast<Register *>(ops.front().get())->reg();
    uint16_t SR = DR;

    if (ops.size() == 2) {
      SR = static_cast<Register *>(ops[1].get())->reg();
    }

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    bin |= (DR << 9) | (SR << 6);

    setAssembled(AssembledToken(
        bin, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: "
                         "<{4}s} NOT R{5:d} "
                         "R{6:d}",
                         programCounter++, bin, line(),
                         sym == symbols.end() ? "" : sym->second.name(), width,
                         DR, SR)));

    bin = 0x1021 | (DR << 9) | (SR << 6);

    asAssembled.emplace_back(
        bin,
        fmt::format(
            "({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} ADD R{5:d} "
            "R{6:d} #1",
            programCounter++, bin, line(), "", width, DR, SR));
  }

  word memoryRequired() const final { return 2_words; }
};
} // namespace Token
} // namespace Lexer

#endif