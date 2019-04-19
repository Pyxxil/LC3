#include "Match.hpp"

namespace Lexer {
Match::Match(TokenType t) : matches({t}) {}

Match &Match::operator|(const Match &t) {
  matches.insert(std::end(matches), std::begin(t.matches), std::end(t.matches));
  return *this;
}

Match &Match::operator|(TokenType t) {
  matches.push_back(t);
  return *this;
}
} // namespace Lexer