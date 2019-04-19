#ifndef TOKEN_REGISTER_HPP
#define TOKEN_REGISTER_HPP

#include "Token.hpp"

namespace Lexer::Token {

class Register : public Token {
public:
  explicit Register(std::string s, size_t t_line, size_t t_column,
                    const std::string &t_file)
      : Token(std::move(s), t_line, t_column, t_file, Requirements()),
        mReg(static_cast<size_t>(token[1] - 0x30)) {}

  Register(const Register &) = default;
  Register(Register &&) = default;

  Register &operator=(const Register &) = default;
  Register &operator=(Register &&) = default;

  TokenType token_type() const final { return REGISTER; }

  void assemble(uint16_t &program_counter, size_t width,
                const std::map<std::string, Symbol> &symbol,
                const std::string &sym) override {}

  auto reg() const { return mReg; }

private:
  size_t mReg{};
};

} // namespace Lexer::Token

#endif
