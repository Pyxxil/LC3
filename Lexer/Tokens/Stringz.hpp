#ifndef TOKEN_STRINGZ_HPP
#define TOKEN_STRINGZ_HPP

#include <numeric>

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Stringz : public Token {
public:
  Stringz(const std::string &t, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(1, {Match(STRING)}, static_cast<size_t>(-1u))) {}

  Stringz(const Stringz &) = default;
  Stringz(Stringz &&) = default;

  Stringz &operator=(const Stringz &) = default;
  Stringz &operator=(Stringz &&) = default;

  TokenType token_type() const final { return STRINGZ; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const std::vector<std::unique_ptr<Token>> &ops = operands();

    auto sym = std::find_if(symbols.begin(), symbols.end(),
                            [programCounter](const auto &sym) {
                              return sym.second.address() == programCounter;
                            });

    const auto &firstString =
        static_cast<String *>(ops.front().get())->true_token();
    auto len = firstString.length();

    set_assembled(AssembledToken(
        static_cast<int16_t>(firstString.front()),
        fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                    ".FILL 0x{5:04X}",
                    programCounter++, static_cast<int16_t>(firstString.front()),
                    line(), sym == symbols.end() ? "" : sym->second.name(),
                    width, static_cast<int16_t>(firstString.front()))));

    for (auto idx = 1; idx < len; ++idx) {
      as_assembled.emplace_back(
          static_cast<uint16_t>(firstString[idx]),
          fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                      ".FILL 0x{5:04X}",
                      programCounter++, static_cast<int16_t>(firstString[idx]),
                      line(), std::string{}, width,
                      static_cast<int16_t>(firstString[idx])));
    }

    as_assembled.emplace_back(
        0, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                       ".FILL 0x0000",
                       programCounter++, 0, line(), std::string{}, width));

    for (auto idx = 1; idx < ops.size(); ++idx) {
      for (auto chr : static_cast<String *>(ops[idx].get())->true_token()) {
        as_assembled.emplace_back(
            static_cast<uint16_t>(chr),
            fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                        ".FILL 0x{5:04X}",
                        programCounter++, static_cast<int16_t>(chr), line(),
                        std::string{}, width, static_cast<int16_t>(chr)));
      }
      as_assembled.emplace_back(
          0, fmt::format("({0:0>4X}) {1:0>4X} {1:0>16b} ({2: >4d}) {3: <{4}s} "
                         ".FILL 0x0000",
                         programCounter++, 0, line(), std::string{}, width));
    }
  }

  word memory_required() const override {
    return static_cast<word>(std::accumulate(
        operands().begin(), operands().end(), 0, [](int sum, auto &&str) {
          return sum +
                 static_cast<String *>(str.get())->Token::get_token().length() +
                 1;
        }));
  }

private:
}; // namespace Token
} // namespace Token
} // namespace Lexer

#endif