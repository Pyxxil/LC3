#ifndef TOKEN_DECIMAL_HPP
#define TOKEN_DECIMAL_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
class Decimal : public Immediate
{
public:
  explicit Decimal(std::string s)
    : Immediate(std::move(s))
  {}

  Decimal(const Decimal&) = default;
  Decimal(Decimal&&) noexcept = default;

  Decimal& operator=(const Decimal&) = default;
  Decimal& operator=(Decimal&&) noexcept = default;

  ~Decimal() override = default;

private:
};
}
}

#endif
