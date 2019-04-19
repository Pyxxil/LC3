#ifndef TOKEN_JMPT_HPP
#define TOKEN_JMPT_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Jmpt : public Token {
public:
  explicit Jmpt(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file);

  Jmpt(const Jmpt &) = default;
  Jmpt(Jmpt &&) = default;

  Jmpt &operator=(const Jmpt &) = default;
  Jmpt &operator=(Jmpt &&) = default;

  TokenType token_type() const final { return JMPT; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif