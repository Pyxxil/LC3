#ifndef TOKEN_BR_HPP
#define TOKEN_BR_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Br : public Token {
public:
  Br(std::string t, bool n, bool z, bool p, size_t t_line, size_t t_column,
     const std::string &t_file);

  Br(const Br &) = default;
  Br(Br &&) = default;

  Br &operator=(const Br &) = default;
  Br &operator=(Br &&) = default;

  TokenType token_type() const final { return BR; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }

private:
  bool N;
  bool Z;
  bool P;
};

} // namespace Lexer::Token

#endif