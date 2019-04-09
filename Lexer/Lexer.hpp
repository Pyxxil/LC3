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

bool file_exists(const std::string &file);

namespace {
const Match requires_zero = Match(TokenType::LABEL) | Match(TokenType::RET) |
                            Match(TokenType::END) | Match(TokenType::HALT) |
                            Match(TokenType::PUTS) | Match(TokenType::OUT) |
                            Match(TokenType::GETC) | Match(TokenType::IN) |
                            Match(TokenType::PUTC) | Match(TokenType::RTI)
#ifdef KEEP_COMMENTS
                            | Match(TokenType::COMMENT)
#endif
    ;
} // namespace

class Lexer {
public:
  Lexer() : m_tokenizer(*this, m_file) {}
  explicit Lexer(File file, bool warnings_are_errors = false)
      : m_file(std::move(file)), from_file(true),
        treat_warnings_as_errors(warnings_are_errors),
        m_tokenizer(*this, m_file) {
    if (file_exists(m_file.name())) {
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(),
          "File is already open (probable recursive include)", m_file.name(),
          0);
      error();
      return;
    }

    if (m_file.is_failure()) {
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(),
          "Unable to open file", m_file.name(), 0);
      error();
      return;
    }

    open_files.emplace_back(m_file.name());

    while (m_file.next_line()) {
      auto &&l_tokens = m_tokenizer.tokenize_line(Line(m_file.line()));
      tokens.insert(std::end(tokens), std::make_move_iterator(l_tokens.begin()),
                    std::make_move_iterator(l_tokens.end()));
    }
  }

  ~Lexer() {
    if (const auto it =
            std::find(open_files.begin(), open_files.end(), m_file.name());
        it != open_files.end()) {
      open_files.erase(it);
    }
  }

  Lexer(const Lexer &) = default;
  Lexer(Lexer &&) = default;

  Lexer &operator=(const Lexer &) = default;
  Lexer &operator=(Lexer &&) = default;

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
    for (const auto &token : lexer.tokens) {
      os << token->AST();
    }

    os << '}';
    return os;
  }

  void for_each(const std::function<void(const Token::Token &)> &f) {
    Algorithm::each(tokens.cbegin(), tokens.cend(), [&f](auto &&t) { f(*t); });
  }

  auto is_okay() const { return m_okay; }
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
    // DEBUG("Tokens size: {}", tokens.size());
    const Match requires_zero = Match(TokenType::LABEL) |
                                Match(TokenType::RET) | Match(TokenType::END) |
                                Match(TokenType::HALT) |
                                Match(TokenType::PUTS) | Match(TokenType::OUT) |
                                Match(TokenType::GETC) | Match(TokenType::IN) |
                                Match(TokenType::PUTC) | Match(TokenType::RTI)
#ifdef KEEP_COMMENTS
                                | Match(TokenType::COMMENT)
#endif
        ;

    for (idx = 0; idx < tokens.size(); ++idx) {
      DEBUG("Found token {}", tokens[idx]->get_token());
      auto &token = *(tokens[idx]);
      const auto &requirements = token.requirements();

      for (auto &&tok : requirements.consume(tokens, idx, m_file)) {
        DEBUG("Consumed - {}", tok->get_token());
        token.add_operand(std::move(tok));
      }

      if (const auto tokens_removed = tokens[idx]->operands().size();
          tokens_removed > 0) {
        const auto begin = tokens.cbegin() + idx + 1;
        tokens.erase(begin, begin + tokens_removed);
      }

      if (!requirements.are_satisfied()) {
        // No need to push a diagnostic here, as Requirements::consume will push
        // the diagnostic for us.
        error();
        ++idx; // Skip the next token, because otherwise it's luikely that the
               // below else if will error out twice for that token.
      } else if (requirements.count() == 0) {
        if (!(requires_zero & token.token_type())) {
          Notification::error_notifications << Diagnostics::Diagnostic(
              std::make_unique<Diagnostics::DiagnosticHighlighter>(
                  token.column(), token.get_token().length(),
                  m_file.line(token.line() - 1)),
              fmt::format("Expected Label, Instruction, or Directive, but "
                          "found '{}' (type {}) instead.",
                          token, token.token_type()),
              token.file(), token.line());
          error();
        }
#ifdef ADDONS
      } else if (token.token_type() == TokenType::INCLUDE) {
        has_includes = true;
#endif
      }
    }

#ifdef ADDONS
    std::vector<std::unique_ptr<Token::Token>> _tokens;
    _tokens.reserve(tokens.capacity());
    for (auto &&token : tokens) {
      DEBUG("Working on token: {}", token->get_token());
      if (token->token_type() == TokenType::INCLUDE) {
        DEBUG("Found this: {}", token->operands().size());
        const auto &file_name = token->operands().front()->get_token();
        Lexer lexer(File{file_name.substr(1, file_name.length() - 2)});
        lexer.lex();

        // it = tokens.erase(it);
        if (m_okay = lexer.is_okay(); m_okay) {
          // TODO: This annoyingly means we will look over every token inside
          // the new tokens as well, which is done by the child lexer...
          _tokens.insert(std::end(_tokens),
                         std::make_move_iterator(std::begin(lexer.tokens)),
                         std::make_move_iterator(std::end(lexer.tokens)));
        }
      } else {
        _tokens.emplace_back(std::move(token));
      }
    }
    tokens = std::move(_tokens);
#endif
  }

  /* Consume a range of tokens from the held tokens
   *
   * @param begin The lowest number of acceptable tokens
   * @param end The highest number of acceptable tokens
   * @return The consumed tokens if there are enough of them, otherwise an
   * empty vector.
   */
  auto consume_range(size_t begin, size_t end) {
    std::vector<std::unique_ptr<Token::Token>> consumed;

    if (idx + begin > tokens.size()) {
      return consumed;
    }

    for (std::size_t i = 0; i < end && idx < tokens.size(); ++i) {
      consumed.emplace_back(std::move(tokens[idx++]));
    }

    return consumed;
  }

  /* Consume count tokens from the held tokens.
   *
   * @param count The number of tokens to consume
   * @return The consumed tokens if there are enough of them, otherwise an
   * empty vector
   */
  auto consume(std::size_t count) {
    if (idx + count > tokens.size()) {
      // Should this be handled here, or inside the caller?
      return std::vector<std::unique_ptr<Token::Token>>();
    }

    return consume_range(count, count);
  }

  void clear() { tokens.clear(); }

  void set_file(File file) {
    m_file = std::move(file);
    from_file = true;
  }

  void unset_file() { from_file = false; }

  std::vector<std::unique_ptr<Token::Token>> tokens{};

  static std::vector<std::string> open_files;

private:
  File m_file{};
  bool from_file{false};
  bool treat_warnings_as_errors{false};
  std::size_t idx{0};
  bool m_okay{true};
  std::size_t error_count{0};
  Tokenizer::Tokenizer m_tokenizer;
  bool has_includes{false};
};

bool file_exists(const std::string &file) {
  return Algorithm::any(Lexer::open_files.cbegin(), Lexer::open_files.cend(),
                        [&file](auto &&f) { return f == file; });
}

template <> Lexer &Lexer::operator<<<std::string_view>(std::string_view s) {
  // DEBUG("Passed in line '{}'", s);
  auto &&l_tokens = m_tokenizer.tokenize_line(Line(std::string(s)));
  tokens.insert(std::end(tokens), std::make_move_iterator(l_tokens.begin()),
                std::make_move_iterator(l_tokens.end()));
  return *this;
} // namespace Lexer

template <> Lexer &Lexer::operator<<<std::string>(std::string s) {
  // DEBUG("Passed in line '{}'", s);
  auto &&l_tokens = m_tokenizer.tokenize_line(Line(s));
  tokens.insert(std::end(tokens), std::make_move_iterator(l_tokens.begin()),
                std::make_move_iterator(l_tokens.end()));
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
  tokenizer->lexer().error();
}

void throw_warning(Lexer::Tokenizer::Tokenizer *tokenizer,
                   Diagnostics::Diagnostic warning) {
  Notification::warning_notifications << std::move(warning);
  tokenizer->lexer().warn();
}

std::vector<std::string> Lexer::Lexer::open_files{};

#endif
