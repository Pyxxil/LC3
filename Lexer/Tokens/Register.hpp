#ifndef TOKEN_REGISTER_HPP
#define TOKEN_REGISTER_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class Register : public Token {
public:
  explicit Register(std::string s, size_t tLine, size_t tColumn,
                    const std::string &tFile)
      : Token(std::move(s), tLine, tColumn, tFile, Requirements()),
        mReg(static_cast<size_t>(token[1] - 0x30)) {}

  Register(const Register &) = default;
  Register(Register &&) noexcept = default;

  Register &operator=(const Register &) = default;
  Register &operator=(Register &&) noexcept = default;

  TokenType tokenType() const final { return REGISTER; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbol) override {}

  auto reg() const { return mReg; }

private:
  size_t mReg{};
};
} // namespace Token
} // namespace Lexer

#endif
