#ifndef TOKEN_IN_HPP
#define TOKEN_IN_HPP

#include "Token.hpp"

namespace Lexer::Token {

class In : public Token {
public:
  explicit In(std::string t, size_t t_line, size_t t_column,
              const std::string &t_file);

  In(const In &) = default;
  In(In &&) = default;

  In &operator=(const In &) = default;
  In &operator=(In &&) = default;

  TokenType token_type() const final { return IN; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif