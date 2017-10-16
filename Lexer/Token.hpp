#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <ostream>
#include <string>

namespace Lexer {
class Token
{
public:
  explicit Token(std::string t_token)
    : token(t_token)
  {}

  Token(const Token&) = default;
  Token(Token&&) noexcept = default;

  Token& operator=(const Token&) = default;
  Token& operator=(Token&&) noexcept = default;

  ~Token() = default;

  friend std::ostream& operator<<(std::ostream& o, const Token& t)
  {
    return o << t.token;
  }

private:
  std::string token;
};
}

#endif