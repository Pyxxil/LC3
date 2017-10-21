#ifndef TOKEN_BINARY_HPP
#define TOKEN_BINARY_HPP

#include "Immediate.hpp"

namespace Lexer {
namespace Token {
class Binary : public Immediate
{
public:
  explicit Binary(std::string s)
    : Immediate(std::move(s))
  {}

  Binary(const Binary&) = default;
  Binary(Binary&&) noexcept = default;

  Binary& operator=(const Binary&) = default;
  Binary& operator=(Binary&&) noexcept = default;

  ~Binary() override = default;

private:
};
}
}

#endif
