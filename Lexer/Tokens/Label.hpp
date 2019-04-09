#ifndef TOKEN_LABEL_HPP
#define TOKEN_LABEL_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Label : public Token {
public:
  explicit Label(std::string s, size_t t_line, size_t t_column,
                 const std::string &t_file)
      : Token(std::move(s), t_line, t_column,t_file) {}

  Label(const Label &) = default;
  Label(Label &&) = default;

  Label &operator=(const Label &) = default;
  Label &operator=(Label &&) = default;

  TokenType token_type() const final { return LABEL; }

  word memory_required() const override { return 0_word; }

private:
};
} // namespace Token
} // namespace Lexer

#endif
