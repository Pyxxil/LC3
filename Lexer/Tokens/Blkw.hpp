#ifndef TOKEN_BLKW_HPP
#define TOKEN_BLKW_HPP

#include "Immediate.hpp"

namespace Lexer::Token {

class Blkw : public Token {
public:
  explicit Blkw(std::string t, size_t t_line, size_t t_column,
                const std::string &t_file);

  Blkw(const Blkw &) = default;
  Blkw(Blkw &&) = default;

  Blkw &operator=(const Blkw &) = default;
  Blkw &operator=(Blkw &&) = default;

  TokenType token_type() const final { return BLKW; }

  void assemble(int16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const final {
    return static_cast<Immediate *>(operands().front().get())->value();
  }
};

} // namespace Lexer::Token

#endif