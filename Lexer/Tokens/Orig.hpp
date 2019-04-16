#ifndef TOKEN_ORIG_HPP
#define TOKEN_ORIG_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Orig : public Token {
public:
  explicit Orig(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file);

  Orig(const Orig &) = default;
  Orig(Orig &&) = default;

  Orig &operator=(const Orig &) = default;
  Orig &operator=(Orig &&) = default;

  TokenType token_type() const final { return ORIG; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif