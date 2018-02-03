#ifndef COMMENT_HPP
#define COMMENT_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
#ifdef KEEP_COMMENTS
class Comment : public Token {
public:
  explicit Comment(std::string t, size_t tLine, size_t tColumn,
                   const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile, Requirements()) {}

  Comment(const Comment &) = default;
  Comment(Comment &&) noexcept = default;

  Comment &operator=(const Comment &) = default;
  Comment &operator=(Comment &&) noexcept = default;

  TokenType tokenType() const final { return COMMENT; }

  void assemble() override { Token::assemble(); }
};
#endif
} // namespace Token
} // namespace Lexer

#endif