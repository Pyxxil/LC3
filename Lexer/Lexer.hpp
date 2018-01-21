#ifndef LEXER_HPP
#define LEXER_HPP

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "Debug.hpp"

#include "File.hpp"
#include "Line.hpp"
#include "Notifier.hpp"
#include "Token.hpp"
#include "Tokenizer.hpp"

namespace Lexer {
class Lexer {
public:
  Lexer() : m_tokenizer(this, &m_file) {}
  explicit Lexer(File file, bool warnings_are_errors = false)
      : m_file(std::move(file)), from_file(true), m_tokenizer(this, &m_file),
        treat_warnings_as_errors(warnings_are_errors) {
    while (m_file.next_line()) {
      auto &&l_tokens = m_tokenizer.tokenize_line(Line(m_file.line()));
      for (auto &&token : l_tokens) {
        tokens.emplace_back(token);
      }
    }
  }

  Lexer(const Lexer &) = default;
  Lexer(Lexer &&) = default;

  Lexer &operator=(const Lexer &) = default;
  Lexer &operator=(Lexer &&) = default;

  ~Lexer() {
    for (Token::Token *token : tokens) {
      delete token;
    }
  }

  template <typename T,
            typename = std::enable_if<std::is_base_of<Token::Token, T>::value>>
  Lexer &operator<<(T s) {
    tokens.emplace_back(new T(s));
    return *this;
  }

  template <typename OStream>
  friend OStream &operator<<(OStream &os, const Lexer &lexer) {
    if (lexer.from_file)
      os << "Tokens for file " << lexer.m_file.name() << '\n';
    if (lexer.tokens.empty())
      return os << "{}";

    os << "{\n";
    for (const auto token : lexer.tokens) {
      os << token->AST();
    }

    os << '}';
    return os;
  }

  void for_each(const std::function<void(const Token::Token &)> &f) {
    for (auto &&t : tokens) {
      f(*t);
    }
  }

  bool okay() const { return m_okay; }
  void error() {
    m_okay = false;
    ++error_count;
  }
  void warn() {
    if (treat_warnings_as_errors) {
      error();
    }
  }

  /*! Lex the current tokens
   *
   */
  void lex() {
    DEBUG("Tokens size: {}", tokens.size());
    for (idx = 0; idx < tokens.size(); ++idx) {
      DEBUG("Found token {}", tokens[idx]->get_token());
      auto &requirements = tokens[idx]->requirements();
      for (auto token : requirements.consume(tokens, idx)) {
        DEBUG("Consumed - {}", token->get_token());
        tokens[idx]->add_operand(token);
      }

      if (!requirements.are_satisfied()) {
        DEBUG("Did not satisfy requirements for token {}",
              tokens[idx]->get_token());
        continue;
      }

      const auto tokens_removed = tokens[idx]->operands().size();
      tokens.erase(tokens.cbegin() + idx + 1,
                   tokens.cbegin() + idx + tokens_removed + 1);
    }
  }

  /* Consume a range of tokens from the held tokens
   *
   * @param begin The lowest number of acceptable tokens
   * @param end The highest number of acceptable tokens
   * @return The consumed tokens if there are enough of them, otherwise an empty
   * vector.
   */
  auto consume_range(std::size_t begin, std::size_t end) {
    std::vector<Token::Token *> consumed;

    if (idx + begin > tokens.size()) {
      return consumed;
    }

    for (std::size_t i = 0; i < end && idx < tokens.size(); ++i) {
      consumed.emplace_back(tokens[idx++]);
    }

    return consumed;
  }

  /* Consume count tokens from the held tokens.
   *
   * @param count The number of tokens to consume
   * @return The consumed tokens if there are enough of them, otherwise an empty
   * vector
   */
  auto consume(std::size_t count) {
    if (idx + count > tokens.size()) {
      // Should this be handled here, or inside the caller?
      return std::vector<Token::Token *>();
    }

    return consume_range(count, count);
  }

  void clear() {
    for (Token::Token *token : tokens) {
      delete token;
    }
    tokens.clear();
  }

  void set_file(File file) {
    m_file = std::move(file);
    from_file = true;
  }

  void unset_file() { from_file = false; }

  std::vector<Token::Token *> tokens{};

private:
  File m_file{};
  bool from_file{false};
  bool treat_warnings_as_errors{false};
  std::size_t idx{0};
  bool m_okay{true};
  std::size_t error_count{0};
  Tokenizer::Tokenizer m_tokenizer;
};

template <> Lexer &Lexer::operator<<<std::string_view>(std::string_view s) {
  DEBUG("Passed in line '{}'", s);
  auto &&l_tokens = m_tokenizer.tokenize_line(Line(s));
  for (auto &&token : l_tokens) {
    tokens.emplace_back(token);
  }
  return *this;
}

template <> Lexer &Lexer::operator<<<std::string>(std::string s) {
  DEBUG("Passed in line '{}'", s);
  auto &&l_tokens = m_tokenizer.tokenize_line(Line(s));
  for (auto &&token : l_tokens) {
    tokens.emplace_back(token);
  }
  return *this;
}

template <> Lexer &Lexer::operator<<<File>(File file) {
  set_file(std::move(file));

  while (m_file.next_line()) {
    *this << m_file.line();
  }

  unset_file();
  return *this;
}

} // namespace Lexer

void throw_error(Lexer::Tokenizer::Tokenizer *tokenizer,
                 Diagnostics::Diagnostic error) {
  Notification::error_notifications << std::move(error);
  tokenizer->lexer()->error();
}

void throw_warning(Lexer::Tokenizer::Tokenizer *tokenizer,
                   Diagnostics::Diagnostic warning) {
  Notification::warning_notifications << std::move(warning);
  tokenizer->lexer()->warn();
}

#endif