#ifndef TOKEN_OCTAL_HPP
#define TOKEN_OCTAL_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
#ifdef ADDONS
class Octal : public Immediate
{
public:
  explicit Octal(std::string s)
    : Immediate(std::move(s))
  {}

  Octal(const Octal&) = default;
  Octal(Octal&&) noexcept = default;

  Octal& operator=(const Octal&) = default;
  Octal& operator=(Octal&&) noexcept = default;

  ~Octal() override = default;

private:
};
#endif
}
}

#endif
