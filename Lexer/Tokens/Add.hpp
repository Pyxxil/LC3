#ifndef TOKEN_ADD_HPP
#define TOKEN_ADD_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Add : public Token {
public:
  explicit Add(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Add(const Add &) = default;
  Add(Add &&) = default;

  Add &operator=(const Add &) = default;
  Add &operator=(Add &&) = default;

  TokenType token_type() const final { return ADD; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override { return 1_word; }
};

} // namespace Lexer::Token

#endif