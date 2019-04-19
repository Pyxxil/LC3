#ifndef TOKEN_ST_HPP
#define TOKEN_ST_HPP

#include "Token.hpp"

namespace Lexer::Token {

class St : public Token {
public:
  explicit St(std::string t, size_t t_line, size_t t_column,
              const std::string &t_file);

  St(const St &) = default;
  St(St &&) = default;

  St &operator=(const St &) = default;
  St &operator=(St &&) = default;

  TokenType token_type() const final { return ST; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif