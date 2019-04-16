#ifndef TOKEN_BINARY_HPP
#define TOKEN_BINARY_HPP

#include "Immediate.hpp"

namespace Lexer::Token {

class Binary : public Immediate {
public:
  explicit Binary(std::string s, size_t t_line, size_t t_column,
                  const std::string &t_file, bool is_negative = false);

  Binary(const Binary &) = default;
  Binary(Binary &&) = default;

  Binary &operator=(const Binary &) = default;
  Binary &operator=(Binary &&) = default;
};

} // namespace Lexer::Token

#endif
