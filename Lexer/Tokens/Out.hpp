#ifndef TOKEN_OUT_HPP
#define TOKEN_OUT_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Out : public Token {
public:
  Out(std::string t, size_t t_line, size_t t_column, const std::string &t_file);

  Out(const Out &) = default;
  Out(Out &&) = default;

  Out &operator=(const Out &) = default;
  Out &operator=(Out &&) = default;

  TokenType token_type() const final { return OUT; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif