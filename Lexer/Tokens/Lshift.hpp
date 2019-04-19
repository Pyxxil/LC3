#ifndef TOKEN_LSHIFT_HPP
#define TOKEN_LSHIFT_HPP

#include "Immediate.hpp"

namespace Lexer::Token {
#ifdef ADDONS
class Lshift : public Token {
public:
  Lshift(std::string t, size_t t_line, size_t t_column,
         const std::string &t_file);

  Lshift(const Lshift &) = default;
  Lshift(Lshift &&) = default;

  Lshift &operator=(const Lshift &) = default;
  Lshift &operator=(Lshift &&) = default;

  TokenType token_type() const final { return LSHIFT; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override {
    return static_cast<word>(
        static_cast<Immediate *>(operands().back().get())->value());
  }
};
#endif
} // namespace Lexer::Token

#endif