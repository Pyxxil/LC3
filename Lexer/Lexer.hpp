#ifndef LEXER_HPP
#define LEXER_HPP

#include <functional>
#include <vector>

#include "Token.hpp"

namespace Lexer {
class Lexer
{
public:
  Lexer() = default;

  Lexer(const Lexer&) = default;
  Lexer(Lexer&&) = default;

  Lexer& operator=(const Lexer&) = default;
  Lexer& operator=(Lexer&&) = default;

  ~Lexer() = default;

  Lexer& operator<<(Token t)
  {
    tokens.emplace_back(t);
    return *this;
  }

  void for_each(std::function<void(const Token&)> f)
  {
    for (auto&& t : tokens) {
      f(t);
    }
  }

private:
  std::vector<Token> tokens;
};
}

#endif