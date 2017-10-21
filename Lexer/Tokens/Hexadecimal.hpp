#ifndef TOKEN_HEXADECIMAL_HPP
#define TOKEN_HEXADECIMAL_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
class Hexadecimal : public Immediate
{
public:
  explicit Hexadecimal(std::string s)
    : Immediate(std::move(s))
  {}

  Hexadecimal(const Hexadecimal&) = default;
  Hexadecimal(Hexadecimal&&) noexcept = default;

  Hexadecimal& operator=(const Hexadecimal&) = default;
  Hexadecimal& operator=(Hexadecimal&&) noexcept = default;

  ~Hexadecimal() override = default;

private:
};
}
}

#endif
