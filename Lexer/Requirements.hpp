#ifndef REQUIREMENTS_HPP
#define REQUIREMENTS_HPP

#include <vector>

#include "File.hpp"
#include "Match.hpp"

namespace Lexer::Token {

class Token;

class Requirements {
public:
  Requirements() = default;
  explicit Requirements(size_t _min, std::vector<Match> operands = {},
                        size_t _max = 0);

  Requirements(const Requirements &) = default;
  Requirements(Requirements &&) = default;

  Requirements &operator=(const Requirements &) = default;
  Requirements &operator=(Requirements &&) = default;

  ~Requirements() = default;

  size_t count() const;

  const std::vector<Match> &argument_requirements() const;

  std::vector<std::unique_ptr<Token>>
  consume(std::vector<std::unique_ptr<Token>> &tokens, std::size_t index,
          const File &file) const;

  auto are_satisfied() const { return satisfied; }

private:
  const size_t min{};
  const size_t max{};
  const std::vector<Match> arguments{};
  mutable bool satisfied{false};
};

} // namespace Lexer::Token

#endif