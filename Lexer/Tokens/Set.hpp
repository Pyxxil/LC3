#ifndef TOKEN_SET_HPP
#define TOKEN_SET_HPP

#include "Immediate.hpp"

namespace Lexer::Token {
#ifdef ADDONS
class Set : public Token {
public:
  explicit Set(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Set(const Set &) = default;
  Set(Set &&) = default;

  Set &operator=(const Set &) = default;
  Set &operator=(Set &&) = default;

  TokenType token_type() const final { return SET; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override {
    const auto immediate_value =
        static_cast<Immediate *>(operands().back().get())->value();
    return static_cast<word>(
        (immediate_value > 15 || immediate_value < -16) ? 3 : 2);
  }
};
#endif
} // namespace Lexer::Token

#endif