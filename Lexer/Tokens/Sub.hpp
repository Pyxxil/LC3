#ifndef TOKEN_SUB_HPP
#define TOKEN_SUB_HPP

#include "Register.hpp"

namespace Lexer::Token {
#ifdef ADDONS
class Sub : public Token {
public:
  explicit Sub(std::string t, size_t t_line, size_t t_column,
               const std::string &t_file);

  Sub(const Sub &) = default;
  Sub(Sub &&) = default;

  Sub &operator=(const Sub &) = default;
  Sub &operator=(Sub &&) = default;

  TokenType token_type() const final { return SUB; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols,
                const std::string &sym) override;

  word memory_required() const override {
    const size_t first_register_idx = operands().size() - 2;
    const size_t second_register_idx = operands().size() - 1;

    const auto second_register =
        static_cast<Register *>(operands()[second_register_idx].get())->reg();

    if (static_cast<Register *>(operands()[first_register_idx].get())->reg() ==
        second_register) {
      return 1_word;
    } else if (static_cast<Register *>(operands().front().get())->reg() !=
               second_register) {
      return 5_words;
    } else {
      return 3_words;
    }
  }
};
#endif
} // namespace Lexer::Token

#endif