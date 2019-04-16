#include "Requirements.hpp"
#include "Token.hpp"

namespace Lexer::Token {

Requirements::Requirements(size_t _min, std::vector<Match> operands,
                           size_t _max)
    : min(_min), max(_min > _max ? _min : _max),
      arguments(std::move(operands)) {
  assert(arguments.size() >= min && arguments.size() <= max);
}

const std::vector<Match> &Requirements::argument_requirements() const {
  return arguments;
}

size_t Requirements::count() const { return min; }

/*! Consume tokens from the tokens the lexer contains according to our
 *  requirements.
 *
 * If a token doesn't match the requirements then the index will be reset and
 * an empty vector will be returned, and we will stay unsatisfied.
 *
 * @param tokens The tokens to consume from
 * @param index The starting index into those tokens (this is updated)
 * @return The consumed tokens
 */
std::vector<std::unique_ptr<Token>>
Requirements::consume(std::vector<std::unique_ptr<Token>> &tokens, size_t index,
                      const File &file) const {
  std::vector<std::unique_ptr<Token>> consumed;
  if (0 == min) {
    DEBUG("Trying to consume for a token which takes no operands", "");
    satisfied = true;
    return consumed;
  }

  if (tokens.size() <= index + min) {
    DEBUG("Not enough tokens ({} > {})", min, tokens.size() - index - 1);
    return consumed;
  }

  if (max == -1u) {
    // Need to match at least one of the tokens
    if (!(arguments[0] & tokens[index + 1]->token_type())) {
      auto &&token = tokens[index + 1];
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(
              token->column(), token->get_token().length(),
              file.line(token->line() - 1)),
          fmt::format("Expected {}, but found '{}' (with type {})",
                      arguments[0], token->get_token(), token->token_type()),
          token->file(), token->line());
      return consumed;
    }

    consumed.emplace_back(std::move(tokens[index + 1]));

    size_t idx = index + 2;
    while (idx < tokens.size() && arguments[0] & tokens[idx]->token_type()) {
      // DEBUG("Found token {}", tokens[idx]->get_token());
      consumed.emplace_back(std::move(tokens[idx++]));
    }

    satisfied = true;
    return consumed;
  }

  const size_t end = index + max;
  for (size_t idx = index, arg_index = 0; idx < end && idx + 1 < tokens.size();
       ++idx, ++arg_index) {
    // DEBUG("Argument type required for this index is {}",
    // arguments[arg_index]);
    const auto &token = tokens[idx + 1];
    if (arguments[arg_index] & tokens[idx + 1]->token_type()) {
      // DEBUG("Valid consumed token: {}, with type {}", token->get_token(),
      //      token->token_type());
      consumed.emplace_back(std::move(tokens[idx + 1]));
    } else if (arg_index >= min && arg_index <= max) {
      // DEBUG("Valid consumption, finished with argument count {}", arg_index);
      break;
    } else {
      // DEBUG("Invalid consumed token: {}, with type {}", token->get_token(),
      // token->token_type());
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(
              token->column(), token->get_token().length(),
              file.line(token->line() - 1)),
          fmt::format("Expected {}, but found '{}' (with type {})",
                      arguments[arg_index], token->get_token(),
                      token->token_type()),
          token->file(), token->line());
      return consumed;
    }
  }

  satisfied = true;
  return consumed;
}

} // namespace Lexer::Token