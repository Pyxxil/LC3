#ifndef MATCH_HPP
#define MATCH_HPP

#include <vector>

#include "Algorithm.hpp"
#include "TokenExtras.hpp"

namespace Lexer {

class Match {
public:
  Match() = default;
  explicit Match(TokenType t);

  Match(const Match &) = default;
  Match(Match &&) noexcept = default;

  Match &operator=(const Match &) = default;
  Match &operator=(Match &&) noexcept = default;

  ~Match() = default;

  template <typename OStream>
  friend OStream &operator<<(OStream &os, const Match &t) {
    Algorithm::enumerate(t.matches.cbegin(), t.matches.cend(),
                         [&os, &t](auto &&token_type, size_t idx) {
                           if (0 == idx) {
                             if (t.matches.size() > 1) {
                               os << "one of (";
                             }
                           } else {
                             os << ", ";
                           }

                           os << token_type;

                           if (t.matches.size() > 1 &&
                               idx == t.matches.size() - 1) {
                             os << ')';
                           }
                         });

    return os;
  }

  Match &operator|(const Match &t);
  Match &operator|(TokenType t);

  bool operator&(const Match &t) const {
    return Algorithm::any(t.matches.begin(), t.matches.cbegin(),
                          [this](auto &&match) { return *this & match; });
  }

  bool operator&(TokenType t) const {
    return Algorithm::any(matches.cbegin(), matches.cend(),
                          [&t](auto &&match) { return match == t; });
  }

private:
  std::vector<TokenType> matches{};
};
} // namespace Lexer

#endif