#ifndef TOKEN_PUTSP_HPP
#define TOKEN_PUTSP_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Putsp : public Token {
public:
  Putsp(std::string t, size_t t_line, size_t t_column,
        const std::string &t_file);

  Putsp(const Putsp &) = default;
  Putsp(Putsp &&) = default;

  Putsp &operator=(const Putsp &) = default;
  Putsp &operator=(Putsp &&) = default;

  TokenType token_type() const final { return PUTSP; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif