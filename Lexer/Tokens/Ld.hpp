#ifndef TOKEN_LD_HPP
#define TOKEN_LD_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Ld : public Token {
public:
  explicit Ld(std::string t, size_t t_line, size_t t_column,
              const std::string &t_file);

  Ld(const Ld &) = default;
  Ld(Ld &&) = default;

  Ld &operator=(const Ld &) = default;
  Ld &operator=(Ld &&) = default;

  TokenType token_type() const final { return LD; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif