#ifndef TOKEN_STRINGZ_HPP
#define TOKEN_STRINGZ_HPP

#include <numeric>

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Stringz : public Token {
public:
  Stringz(std::string t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(STRING)}, -1u)) {}

  Stringz(const Stringz &) = default;
  Stringz(Stringz &&) noexcept = default;

  Stringz &operator=(const Stringz &) = default;
  Stringz &operator=(Stringz &&) noexcept = default;

  TokenType tokenType() const final { return STRINGZ; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const std::vector<std::unique_ptr<Token>> &ops = operands();

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    const auto &firstString =
        static_cast<String *>(ops.front().get())->trueToken();
    auto len = firstString.length();

    setAssembled(AssembledToken(
        static_cast<int16_t>(firstString.front()),
        fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                    ".FILL 0x{5:04X}",
                    programCounter++, static_cast<int16_t>(firstString.front()),
                    line(), sym == symbols.end() ? "" : sym->second.name(),
                    width, static_cast<int16_t>(firstString.front()))));

    for (auto idx = 1; idx < len; ++idx) {
      asAssembled.emplace_back(
          static_cast<int16_t>(firstString[idx]),
          fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                      ".FILL 0x{5:04X}",
                      programCounter++, static_cast<int16_t>(firstString[idx]),
                      line(), std::string{}, width,
                      static_cast<int16_t>(firstString[idx])));
    }

    asAssembled.emplace_back(
        0, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                       ".FILL 0x0000",
                       programCounter++, 0, line(), std::string{}, width));

    for (auto idx = 1; idx < ops.size(); ++idx) {
      for (auto chr : static_cast<String *>(ops[idx].get())->trueToken()) {
        asAssembled.emplace_back(
            static_cast<int16_t>(chr),
            fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                        ".FILL 0x{5:04X}",
                        programCounter++, static_cast<int16_t>(chr), line(),
                        std::string{}, width, static_cast<int16_t>(chr)));
      }
      asAssembled.emplace_back(
          0, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                         ".FILL 0x0000",
                         programCounter++, 0, line(), std::string{}, width));
    }
  }

  word memoryRequired() const override {
    return static_cast<word>(std::accumulate(
        operands().begin(), operands().end(), 0, [](int sum, auto &&str) {
          return sum +
                 static_cast<String *>(str.get())->Token::getToken().length() +
                 1;
        }));
  }

private:
}; // namespace Token
} // namespace Token
} // namespace Lexer

#endif