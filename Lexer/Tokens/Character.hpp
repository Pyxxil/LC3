#ifndef TOKEN_CHARACTER_HPP
#define TOKEN_CHARACTER_HPP

#include "Immediate.hpp"

namespace Lexer::Token {
#ifdef ADDONS
class Character : public Immediate {
public:
  Character(std::string s, size_t t_line, size_t t_column,
            const std::string &t_file);

  Character(const Character &) = default;
  Character(Character &&) = default;

  Character &operator=(const Character &) = default;
  Character &operator=(Character &&) = default;

  const std::string &get_token() const override;

private:
  mutable std::string character{};
};
#endif
} // namespace Lexer::Token

#endif
