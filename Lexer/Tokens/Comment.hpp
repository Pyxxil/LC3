#ifndef COMMENT_HPP
#define COMMENT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
#ifdef KEEP_COMMENTS
class Comment : public Token {
public:
  explicit Comment(std::string t, size_t t_line, size_t t_column,
                   const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file, Requirements()) {}

  Comment(const Comment &) = default;
  Comment(Comment &&) = default;

  Comment &operator=(const Comment &) = default;
  Comment &operator=(Comment &&) = default;

  TokenType token_type() const final { return COMMENT; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbol) override {}
};
#endif
} // namespace Token
} // namespace Lexer

#endif