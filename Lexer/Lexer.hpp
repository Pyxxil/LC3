#ifndef LEXER_HPP
#define LEXER_HPP

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "../Debug.hpp"

#include "File.hpp"
#include "Line.hpp"
#include "Notifier.hpp"
#include "Token.hpp"
#include "Tokenizer.hpp"

namespace Lexer {
class Lexer {
public:
  Lexer() : mTokenizer(*this, mFile) {}
  explicit Lexer(File file, bool warnings_are_errors = false)
      : mFile(std::move(file)), fromFile(true),
        treatWarningsAsErrors(warnings_are_errors), mTokenizer(*this, mFile) {
    if (const auto it =
            std::find(openFiles.begin(), openFiles.end(), mFile.name());
        it != openFiles.end()) {
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(0, 0, ""),
          "File is already open (probable recursive includes)", mFile.name(),
          0);
      error();
      return;
    }

    if (mFile.isFailure()) {
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(0, 0, ""),
          "Unable to open file", mFile.name(), 0);
      error();
      return;
    }

    openFiles.emplace_back(mFile.name());

    while (mFile.nextLine()) {
      auto &&l_tokens = mTokenizer.tokenizeLine(Line(mFile.line()));
      for (auto &&token : l_tokens) {
        tokens.emplace_back(std::move(token));
      }
    }
  }

  ~Lexer() {
    const auto it = std::find(openFiles.begin(), openFiles.end(), mFile.name());
    if (it != openFiles.end()) {
      openFiles.erase(it);
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
    if (lexer.fromFile)
      os << "Tokens for file " << lexer.mFile.name() << '\n';
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
    for (auto &&t : tokens) {
      f(*t);
    }
  }

  bool isOkay() const { return mOkay; }
  void error() {
    mOkay = false;
    ++errorCount;
  }
  void warn() {
    if (treatWarningsAsErrors) {
      error();
    }
  }

  /*! Lex the current tokens
   *
   */
  void lex() {
    DEBUG("Tokens size: {}", tokens.size());
    const Match requiresZero = Match(TokenType::LABEL) | Match(TokenType::RET) |
                               Match(TokenType::END) | Match(TokenType::HALT) |
                               Match(TokenType::PUTS) | Match(TokenType::OUT) |
                               Match(TokenType::GETC) | Match(TokenType::IN) |
                               Match(TokenType::PUTC)
#ifdef KEEP_COMMENTS
                               | Match(TokenType::COMMENT)
#endif
        ;

    for (idx = 0; idx < tokens.size(); ++idx) {
      DEBUG("Found token {}", tokens[idx]->getToken());
      const auto &requirements = tokens[idx]->requirements();

      for (auto &&token : requirements.consume(tokens, idx, mFile)) {
        DEBUG("Consumed - {}", token->getToken());
        tokens[idx]->addOperand(std::move(token));
      }

      if (const auto tokensRemoved = tokens[idx]->operands().size();
          tokensRemoved > 0) {
        tokens.erase(tokens.cbegin() + idx + 1,
                     tokens.cbegin() + idx + tokensRemoved + 1);
      }

      if (!requirements.are_satisfied()) {
        // No need to push a diagnostic here, as Requirements::consume will push
        // the diagnostic for us.
        error();
        ++idx; // Skip the next token, because otherwise it's likely that the
               // below else if will error out twice for that token.
        continue;
      } else if (requirements.count() == 0 &&
                 !(requiresZero & tokens[idx]->tokenType())) {
        const auto &tok = tokens[idx];
        Notification::error_notifications << Diagnostics::Diagnostic(
            std::make_unique<Diagnostics::DiagnosticHighlighter>(
                tok->column(), tok->getToken().length() - 1,
                mFile.line(tok->line() - 1)),
            fmt::format("Expected Label, Instruction, or Directive, but "
                        "found '{}' (type {}) instead.",
                        *tok, tok->tokenType()),
            tok->file(), tok->line());
        error();
        continue;
      }
    }

#ifdef ADDONS
    std::vector<std::unique_ptr<Token::Token>> _tokens{};
    for (std::vector<std::unique_ptr<Token::Token>>::iterator it =
             tokens.begin();
         it != tokens.end(); ++it) {
      if ((*it)->tokenType() == TokenType::INCLUDE) {
        const auto &fileName = (*it)->operands().front()->getToken();
        Lexer lexer(File{fileName.substr(1, fileName.length() - 2)});
        lexer.lex();

        if (mOkay = lexer.isOkay(); mOkay) {
          for (auto &token : lexer.tokens) {
            _tokens.emplace_back(std::move(token));
          }
        }
      } else {
        _tokens.emplace_back(std::move(*it));
      }
    }
    tokens = std::move(_tokens);
#endif
  }

  /* Consume a range of tokens from the held tokens
   *
   * @param begin The lowest number of acceptable tokens
   * @param end The highest number of acceptable tokens
   * @return The consumed tokens if there are enough of them, otherwise an empty
   * vector.
   */
  auto consume_range(std::size_t begin, std::size_t end) {
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
   * @return The consumed tokens if there are enough of them, otherwise an empty
   * vector
   */
  auto consume(std::size_t count) {
    if (idx + count > tokens.size()) {
      // Should this be handled here, or inside the caller?
      return std::vector<std::unique_ptr<Token::Token>>();
    }

    return consume_range(count, count);
  }

  void clear() { tokens.clear(); }

  void setFile(File file) {
    mFile = std::move(file);
    fromFile = true;
  }

  void unset_file() { fromFile = false; }

  std::vector<std::unique_ptr<Token::Token>> tokens{};

  static std::vector<std::string> openFiles;

private:
  File mFile{};
  bool fromFile{false};
  bool treatWarningsAsErrors{false};
  std::size_t idx{0};
  bool mOkay{true};
  std::size_t errorCount{0};
  Tokenizer::Tokenizer mTokenizer;
}; // namespace Lexer

template <> Lexer &Lexer::operator<<<std::string_view>(std::string_view s) {
  DEBUG("Passed in line '{}'", s);
  auto &&l_tokens = mTokenizer.tokenizeLine(Line(s));
  for (auto &&token : l_tokens) {
    tokens.emplace_back(std::move(token));
  }
  return *this;
}

template <> Lexer &Lexer::operator<<<std::string>(std::string s) {
  DEBUG("Passed in line '{}'", s);
  auto &&l_tokens = mTokenizer.tokenizeLine(Line(s));
  for (auto &&token : l_tokens) {
    tokens.emplace_back(std::move(token));
  }
  return *this;
}

template <> Lexer &Lexer::operator<<<File>(File file) {
  setFile(std::move(file));

  while (mFile.nextLine()) {
    *this << mFile.line();
  }

  unset_file();
  return *this;
}

} // namespace Lexer

void throwError(Lexer::Tokenizer::Tokenizer *tokenizer,
                Diagnostics::Diagnostic error) {
  Notification::error_notifications << std::move(error);
  tokenizer->lexer().error();
}

void throwWarning(Lexer::Tokenizer::Tokenizer *tokenizer,
                  Diagnostics::Diagnostic warning) {
  Notification::warning_notifications << std::move(warning);
  tokenizer->lexer().warn();
}

std::vector<std::string> Lexer::Lexer::openFiles{};

#endif