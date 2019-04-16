#ifndef TOKEN_DECIMAL_HPP
#define TOKEN_DECIMAL_HPP

#include "Immediate.hpp"

namespace Lexer::Token {

class Decimal : public Immediate {
public:
  explicit Decimal(std::string s, size_t t_line, size_t t_column,
                   const std::string &t_file,
                   bool is_negative = false); // namespace Token

  Decimal(const Decimal &) = default;
  Decimal(Decimal &&) = default;

  Decimal &operator=(const Decimal &) = default;
  Decimal &operator=(Decimal &&) = default;
};

} // namespace Lexer::Token

#endif
