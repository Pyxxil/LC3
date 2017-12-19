#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cassert>
#include <string>
#include <vector>

#include "Algorithm.hpp"
#include "Debug.hpp"

namespace Lexer {

enum Token_Type {
  NONE = 0,
  ADD,
  AND,
  BR,
  NOT,
  JMP,
  JSR,
  JSRR,
  LD,
  LEA,
  LDI,
  LDR,
  ST,
  STI,
  STR,
  TRAP,
  HALT,
  PUTS,
  PUTSP,
  PUTC,
  GETC,
  OUT,
  IN,
  RTI,
  RET,
  REGISTER,
  IMMEDIATE,
  STRING,
  LABEL,
  STRINGZ,
  FILL,
  BLKW,
  ORIG,
  END,
#ifdef ADDONS
  INCLUDE,
  LSHIFT,
  SET,
  NEG,
  SUB,
#endif
};

const char *TokenTypeString[] = {
    "NONE",    "ADD",     "AND",  "BR",   "NOT",      "JMP",       "JSR",
    "JSRR",    "LD",      "LEA",  "LDI",  "LDR",      "ST",        "STI",
    "STR",     "TRAP",    "HALT", "PUTS", "PUTSP",    "PUTC",      "GETC",
    "OUT",     "IN",      "RTI",  "RET",  "REGISTER", "IMMEDIATE", "STRING",
    "LABEL",   "STRINGZ", "FILL", "BLKW", "ORIG",     "END",
#ifdef ADDONS
    "INCLUDE", "LSHIFT",  "SET",  "NEG",  "SUB"
#endif
};

class Match {
public:
  Match() = default;
  explicit Match(Token_Type t) : matches({t}) {}

  Match(const Match &) = default;
  Match(Match &&) noexcept = default;

  Match &operator=(const Match &) = default;
  Match &operator=(Match &&) noexcept = default;

  ~Match() = default;

  friend std::ostream &operator<<(std::ostream &os, const Match &t) {
    using namespace Algorithm;
    enumerate(t.matches.cbegin(), t.matches.cend(),
              [&os, &t](auto &&token, size_t idx) {
                if (0 == idx) {
                  if (t.matches.size() > 1) {
                    os << "one of (";
                  }
                } else {
                  os << ", ";
                }

                os << TokenTypeString[token] << '(' << token << ')';

                if (t.matches.size() > 1 && idx == t.matches.size() - 1) {
                  os << ')';
                }
              });

    return os;
  }

  Match &operator|(const Match &t) {
    for (auto &&match : t.matches) {
      matches.push_back(match);
    }
    return *this;
  }
  Match &operator|(Token_Type t) {
    matches.push_back(t);
    return *this;
  }

  bool operator&(const Match &t) {
    for (auto &&match : t.matches) {
      if (*this & match) {
        return true;
      }
    }

    return false;
  }

  bool operator&(Token_Type t) {
    for (auto &&match : matches) {
      if (match == t) {
        return true;
      }
    }

    return false;
  }

private:
  std::vector<Token_Type> matches{};
};

namespace Token {
class Token;

class Requirements {
public:
  Requirements() = default;
  explicit Requirements(std::size_t _min, std::vector<Match> operands = {},
                        std::size_t _max = 0)
      : min(_min), max(_min), arguments(std::move(operands)) {
    if (_min > _max) {
      max = _min;
    }

    assert(arguments.size() >= min && arguments.size() <= max);
  }

  Requirements(const Requirements &) = default;
  Requirements(Requirements &&) noexcept = default;

  Requirements &operator=(const Requirements &) = default;
  Requirements &operator=(Requirements &&) noexcept = default;

  ~Requirements() = default;

  auto count() const { return min; }

  const auto &argument_reqs() const { return arguments; }

  std::vector<Token *> consume(const std::vector<Token *> &tokens,
                               std::size_t &index);

  bool are_satisfied() const { return satisfied; }

private:
  std::size_t min{};
  std::size_t max{};
  std::vector<Match> arguments;
  bool satisfied = false;
};

class Token {
public:
  explicit Token(std::string t, Requirements r = Requirements())
      : token(std::move(t)), requirements(std::move(r)) {}

  Token(const Token &) = default;
  Token(Token &&) noexcept = default;

  Token &operator=(const Token &) = default;
  Token &operator=(Token &&) noexcept = default;

  virtual ~Token() {
    for (auto operand : operands) {
      delete operand;
    }
  }

  const auto &getToken() const { return token; }

  virtual Token_Type tokenType() const { return NONE; }

  auto &getRequirements() { return requirements; }

  virtual void assemble() {}

  virtual void negate() {}

protected:
  std::string token;

private:
  Requirements requirements;
  std::vector<Token *> operands{};
};

/*! Consume tokens from the tokens the lexer contains according to our
 * requirements.
 *
 * If a token doesn't match the requirements then the index will be reset and an
 * empty vector will be returned, and we will stay unsatisfied.
 *
 * @param tokens The tokens to consume from
 * @param index The starting index into those tokens (this is updated)
 * @return The consumed tokens
 */
std::vector<Token *> Requirements::consume(const std::vector<Token *> &tokens,
                                           std::size_t &index) {
  std::vector<Token *> consumed;
  if (0 == min) {
    Debug::console->debug(
        "Trying to consume for a token which takes no operands");
    return consumed;
  }

  if (tokens.size() <= index + min) {
    Debug::console->debug("Not enough tokens ({} > {})", min,
                          (tokens.size() - index - 1));
    return consumed;
  }

  const auto starting_index = index;
  for (std::size_t idx = 0; idx < max && index + 1 < tokens.size(); ++idx) {
    Debug::console->debug("Argument type required for this index is {}",
                          arguments[idx]);
    if (arguments[idx] & tokens[index + 1]->tokenType()) {
      Debug::console->debug("Valid consumed token: {}, with type {}",
                            tokens[index + 1]->getToken(),
                            TokenTypeString[tokens[index + 1]->tokenType()]);
      consumed.emplace_back(tokens[++index]);
    } else {
      Debug::console->debug("Invalid consumed token: {}, with type {}",
                            tokens[index + 1]->getToken(),
                            TokenTypeString[tokens[index + 1]->tokenType()]);
      index = starting_index;
      return {};
    }
  }

  satisfied = true;
  return consumed;
}

std::ostream &operator<<(std::ostream &os, const Token &t) {
  return os << t.getToken();
}
} // namespace Token
} // namespace Lexer

#endif