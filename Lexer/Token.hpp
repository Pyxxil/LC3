#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cassert>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Algorithm.hpp"
#include "AssembledToken.hpp"
#include "Debug.hpp"
#include "Diagnostic.hpp"
#include "File.hpp"
#include "Notifier.hpp"
#include "Requirements.hpp"
#include "Symbol.hpp"
#include "TokenExtras.hpp"

#include "fmt/format.h"

namespace Lexer::Token {

class Token {
public:
  Token(std::string t, size_t t_line, size_t t_column, std::string t_file,
        Requirements r = Requirements());

  Token(const Token &) = default;
  Token(Token &&) = default;

  Token &operator=(const Token &) = default;
  Token &operator=(Token &&) = default;

  virtual ~Token() = default;

  constexpr const auto &operands() const { return m_operands; }

  void compile_ast() const;

  const std::string &AST() const;

  virtual const std::string &get_token() const;

  virtual TokenType token_type() const;

  constexpr auto &requirements() const { return m_requirements; }

  void add_operand(std::unique_ptr<Token> operand);

  virtual void assemble(int16_t &programCounter, size_t width,
                        const std::map<std::string, Symbol> &symbols,
                        const std::string &sym) {}
  virtual word memory_required() const { return -1; }

  virtual bool is_too_big() const { return too_big; }

  constexpr const auto &file() const { return m_file; }
  constexpr auto line() const { return at_line; }
  constexpr auto column() const { return at_column; }

  void set_assembled(AssembledToken assembled);
  const std::vector<AssembledToken> &assembled() const;

protected:
  std::string token;
  bool too_big{false};
  std::vector<AssembledToken> as_assembled{};

private:
  const size_t at_line;
  const size_t at_column;
  const std::string m_file;
  Requirements m_requirements;
  std::vector<std::unique_ptr<Token>> m_operands{};
  mutable std::string ast{};
  mutable bool ast_compiled{false};
};

template <typename OStream>
inline OStream &operator<<(OStream &os, const Token &t) {
  return os << t.get_token();
}

} // namespace Lexer::Token

#endif