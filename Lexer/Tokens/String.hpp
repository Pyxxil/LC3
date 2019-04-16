#ifndef TOKEN_STRING_HPP
#define TOKEN_STRING_HPP

#include "Token.hpp"

namespace Lexer::Token {

class String : public Token {
public:
  String(std::string s, size_t t_line, size_t t_column,
         const std::string &t_file);

  String(const String &) = default;
  String(String &&) = default;

  String &operator=(const String &) = default;
  String &operator=(String &&) = default;

  TokenType token_type() const final { return STRING; }

  const std::string &true_token() const { return token; }

  const std::string &get_token() const override;

private:
  mutable std::string string{};
};

} // namespace Lexer::Token

#endif
