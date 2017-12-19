#ifndef LEXER_HPP
#define LEXER_HPP

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "Debug.hpp"

#include "Line.hpp"
#include "Token.hpp"
#include "Tokenizer.hpp"

namespace Lexer {
class Lexer {
public:
  Lexer() = default;

  Lexer(const Lexer &) = default;
  Lexer(Lexer &&) = default;

  Lexer &operator=(const Lexer &) = default;
  Lexer &operator=(Lexer &&) = default;

  ~Lexer() {
    for (Token::Token *token : tokens) {
      delete token;
    }
  }

  Lexer &operator<<(Token::Token *t) {
    tokens.emplace_back(t);
    return *this;
  }

  Lexer &operator<<(std::string_view s) {
    Debug::console->debug("Passed in line '{}'", s);
    auto &&l_tokens = Tokenizer::tokenize_line(Line(s));
    for (auto &&token : l_tokens) {
      tokens.emplace_back(token);
    }
    return *this;
  }

  void for_each(const std::function<void(const Token::Token &)> &f) {
    for (auto &&t : tokens) {
      f(*t);
    }
  }

  /*! Lex the current tokens
   *
   */
  void lex() {
    Debug::console->debug("Tokens size: {}", tokens.size());
    for (idx = 0; idx < tokens.size(); ++idx) {
      Debug::console->debug("Found token {}", tokens[idx]->getToken());
      auto &requirements = tokens[idx]->getRequirements();
      for (auto &token : requirements.consume(tokens, idx)) {
        (void) token;
        Debug::console->debug("Consumed - {}", token->getToken());
      }
      if (!requirements.are_satisfied()) {
        Debug::console->debug("Did not satisfy requirements for token {}",
              tokens[idx]->getToken());
      }
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

  std::vector<Token::Token *> tokens{};

private:
  std::size_t idx{0};
};
} // namespace Lexer

#endif