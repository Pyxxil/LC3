#ifndef TOKEN_FILL_HPP
#define TOKEN_FILL_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Fill : public Token {
public:
  explicit Fill(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file);

  Fill(const Fill &) = default;
  Fill(Fill &&) = default;

  Fill &operator=(const Fill &) = default;
  Fill &operator=(Fill &&) = default;

  TokenType token_type() const final { return FILL; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif