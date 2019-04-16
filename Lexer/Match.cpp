#include "Match.hpp"

namespace Lexer {
Match::Match(TokenType t) : matches({t}) {}

Match &Match::operator|(const Match &t) {
  for (auto &&match : t.matches) {
    matches.push_back(match);
  }
  return *this;
}

Match &Match::operator|(TokenType t) {
  matches.push_back(t);
  return *this;
}
} // namespace Lexer