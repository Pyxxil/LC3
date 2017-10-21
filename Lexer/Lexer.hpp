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
class Lexer
{
public:
  Lexer() = default;

  Lexer(const Lexer&) = default;
  Lexer(Lexer&&) = default;

  Lexer& operator=(const Lexer&) = default;
  Lexer& operator=(Lexer&&) = default;

  ~Lexer() = default;

  Lexer& operator<<(std::shared_ptr<Token::Token> t)
  {
    tokens.emplace_back(std::move(t));
    return *this;
  }

  Lexer& operator<<(Line l)
  {
    Tokenizer tokenizer;
    auto&& l_tokens = tokenizer.tokenize_line(std::move(l));
    for (auto&& token : l_tokens) {
      tokens.emplace_back(std::move(token));
    }
    return *this;
  }

  void for_each(std::function<void(const std::shared_ptr<Token::Token>&)> f)
  {
    for (auto&& t : tokens) {
      f(t);
    }
  }

  /*! Lex the current tokens
   *
   */
  void lex()
  {
    debug("Tokens size: " << tokens.size());
    for (idx = 0; idx < tokens.size(); ++idx) {
      debug("Found token " << tokens[idx]->getToken());
      auto& requirements = tokens[idx]->getRequirements();
      for (auto& token : requirements.consume(tokens, idx)) {
        debug("Consumed - " << token->getToken());
      }
      if (!requirements.are_satisfied()) {
        debug("Did not satisfy requirements for token "
              << tokens[idx]->getToken());
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
  auto consume_range(std::size_t begin, std::size_t end)
  {
    std::vector<std::shared_ptr<Token::Token>> consumed;

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
  auto consume(std::size_t count)
  {
    if (idx + count > tokens.size()) {
      // Should this be handled here, or inside the caller?
      return std::vector<std::shared_ptr<Token::Token>>();
    }

    return consume_range(count, count);
  }

  std::vector<std::shared_ptr<Token::Token>> tokens{};

private:
  std::size_t idx{ 0 };
};
}

#endif