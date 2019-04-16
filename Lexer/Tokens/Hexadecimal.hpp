#ifndef TOKEN_HEXADECIMAL_HPP
#define TOKEN_HEXADECIMAL_HPP

#include "Immediate.hpp"

namespace Lexer::Token {

class Hexadecimal : public Immediate {
public:
  explicit Hexadecimal(std::string s, size_t t_line, size_t t_column,
                       const std::string &t_file, bool is_negative = false);

  Hexadecimal(const Hexadecimal &) = default;
  Hexadecimal(Hexadecimal &&) = default;

  Hexadecimal &operator=(const Hexadecimal &) = default;
  Hexadecimal &operator=(Hexadecimal &&) = default;
};

} // namespace Lexer::Token

#endif
