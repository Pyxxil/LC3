#ifndef TOKEN_RTI_HPP
#define TOKEN_RTI_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Rti : public Token {
public:
  Rti(std::string t, size_t t_line, size_t t_column, const std::string &t_file)
      : Token(std::move(t), t_line, t_column, t_file) {}

  Rti(const Rti &) = default;
  Rti(Rti &&) = default;

  Rti &operator=(const Rti &) = default;
  Rti &operator=(Rti &&) = default;

  TokenType token_type() const final { return RTI; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbol) override {}

  word memory_required() const override { return 1_word; }
};
} // namespace Token
} // namespace Lexer

#endif