#ifndef TOKEN_RET_HPP
#define TOKEN_RET_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Ret : public Token {
public:
  Ret(std::string t, size_t t_line, size_t t_column, const std::string &t_file);

  Ret(const Ret &) = default;
  Ret(Ret &&) = default;

  Ret &operator=(const Ret &) = default;
  Ret &operator=(Ret &&) = default;

  TokenType token_type() const final { return RET; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif