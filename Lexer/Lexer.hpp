#ifndef LEXER_HPP
#define LEXER_HPP

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "../Debug.hpp"

#include "Algorithm.hpp"
#include "File.hpp"
#include "Line.hpp"
#include "Notifier.hpp"
#include "Token.hpp"
#include "Tokenizer.hpp"

namespace Lexer {

extern std::vector<std::string> open_files;

class Lexer {
public:
  Lexer();
  explicit Lexer(File file, bool warnings_are_errors = false);

  ~Lexer();

  Lexer(const Lexer &) = default;
  Lexer(Lexer &&) = default;

  Lexer &operator=(const Lexer &) = default;
  Lexer &operator=(Lexer &&) = default;

  template <typename T,
            typename = std::enable_if<std::is_base_of<Token::Token, T>::value>>
  Lexer &operator<<(T s);

  void each(const std::function<void(const Token::Token &)> &f) const;

  auto is_okay() const { return m_okay; }
  void error();
  void warn();

  template <typename OStream>
  friend OStream &operator<<(OStream &os, const Lexer &lexer) {
    if (lexer.from_file) {
      os << "Tokens for file " << lexer.m_file.name() << '\n';
    }
    if (lexer.tokens.empty()) {
      return os << "{}";
    }

    os << "{\n";
    lexer.each([&os](auto &&token) { os << token.AST(); });
    os << '}';

    return os;
  }

  /*! Lex the current tokens
   *
   */
  void lex();

  /* Consume a range of tokens from the held tokens
   *
   * @param begin The lowest number of acceptable tokens
   * @param end The highest number of acceptable tokens
   * @return The consumed tokens if there are enough of them, otherwise an
   * empty vector.
   */
  std::vector<std::unique_ptr<Token::Token>> consume_range(size_t begin,
                                                           size_t end);

  /* Consume count tokens from the held tokens.
   *
   * @param count The number of tokens to consume
   * @return The consumed tokens if there are enough of them, otherwise an
   * empty vector
   */
  std::vector<std::unique_ptr<Token::Token>> consume(std::size_t count);

  void clear() { tokens.clear(); }

  void set_file(File file);
  void unset_file();

  std::vector<std::unique_ptr<Token::Token>> tokens{};

private:
  File m_file{};
  bool from_file{false};
  bool treat_warnings_as_errors{false};
  std::size_t idx{0};
  bool m_okay{true};
  std::size_t error_count{0};
  Tokenizer::Tokenizer m_tokenizer;
#ifdef ADDONS
  bool has_includes{false};
#endif
};

} // namespace Lexer

#endif
