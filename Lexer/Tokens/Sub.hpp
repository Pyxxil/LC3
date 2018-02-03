#ifndef TOKEN_SUB_HPP
#define TOKEN_SUB_HPP

#include "Token.hpp"

#include "Register.hpp"

namespace Lexer {
namespace Token {
class Sub : public Token {
public:
  explicit Sub(std::string t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2,
                           {Match(TokenType::REGISTER),
                            Match(TokenType::REGISTER),
                            Match(TokenType::REGISTER)},
                           3)) {}

  Sub(const Sub &) = default;
  Sub(Sub &&) noexcept = default;

  Sub &operator=(const Sub &) = default;
  Sub &operator=(Sub &&) noexcept = default;

  TokenType tokenType() const final { return SUB; }

  void assemble() override { Token::assemble(); }

  word memoryRequired() const override {
    const bool have3Operands = operands().size() == 3;
    const size_t firstRegisterIndex = have3Operands ? 1 : 0;
    const size_t secondRegisterIndex = have3Operands ? 2 : 1;

    const auto secondRegister =
        static_cast<Register *>(operands()[secondRegisterIndex].get())->reg();

    if (static_cast<Register *>(operands()[firstRegisterIndex].get())->reg() ==
        secondRegister) {
      return 1_word;
    } else if (!have3Operands &&
               static_cast<Register *>(operands().front().get())->reg() !=
                   secondRegister) {
      return 5_words;
    } else {
      return 3_words;
    }
  }
};
} // namespace Token
} // namespace Lexer

#endif