#ifndef TOKEN_STRINGZ_HPP
#define TOKEN_STRINGZ_HPP

#include "String.hpp"

namespace Lexer::Token {

class Stringz : public Token {
public:
  Stringz(std::string t, size_t t_line, size_t t_column,
          const std::string &t_file);

  Stringz(const Stringz &) = default;
  Stringz(Stringz &&) = default;

  Stringz &operator=(const Stringz &) = default;
  Stringz &operator=(Stringz &&) = default;

  TokenType token_type() const final { return STRINGZ; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override {
    return static_cast<word>(Algorithm::accumulate(
        operands().begin(), operands().end(), 0, [](int sum, auto &&str) {
          return sum +
                 static_cast<String *>(str.get())->Token::get_token().length() +
                 1;
        }));
  }
};

} // namespace Lexer::Token

#endif