#ifndef TOKEN_GETC_HPP
#define TOKEN_GETC_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Getc : public Token {
public:
  explicit Getc(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file);

  Getc(const Getc &) = default;
  Getc(Getc &&) = default;

  Getc &operator=(const Getc &) = default;
  Getc &operator=(Getc &&) = default;

  TokenType token_type() const final { return GETC; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif