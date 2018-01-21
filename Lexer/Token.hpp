#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cassert>
#include <string>
#include <vector>

#include "Algorithm.hpp"
#include "Debug.hpp"
#include "spdlog/fmt/fmt.h"

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

const char *TokenTypeString[] = {"INVALID",
                                 "Instruction ADD",
                                 "Instruction AND",
                                 "Instruction BR",
                                 "Instruction NOT",
                                 "Instruction JMP",
                                 "Instruction JSR",
                                 "Instruction JSRR",
                                 "Instruction LD",
                                 "Instruction LEA",
                                 "Instruction LDI",
                                 "Instruction LDR",
                                 "Instruction ST",
                                 "Instruction STI",
                                 "Instruction STR",
                                 "Instruction TRAP",
                                 "Trap HALT",
                                 "Trap PUTS",
                                 "Trap PUTSP",
                                 "Trap PUTC",
                                 "Trap GETC",
                                 "Trap OUT",
                                 "Trap IN",
                                 "Instruction RTI",
                                 "Pseudo Op RET",
                                 "REGISTER",
                                 "IMMEDIATE",
                                 "STRING",
                                 "LABEL",
                                 "Directive .STRINGZ",
                                 "Directive .FILL",
                                 "Directive .BLKW",
                                 "Directive .ORIG",
                                 "Directive .END",
#ifdef ADDONS
                                 "Directive .INCLUDE",
                                 "LSHIFT",
                                 "Directive .SET",
                                 "Directive .NEG",
                                 "Directive .SUB"
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
              [&os, &t](auto &&token_type, size_t idx) {
                if (0 == idx) {
                  if (t.matches.size() > 1) {
                    os << "one of (";
                  }
                } else {
                  os << ", ";
                }

                os << TokenTypeString[token_type] << '(' << token_type << ')';

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
                               std::size_t index);

  bool are_satisfied() const { return satisfied; }

private:
  std::size_t min{};
  std::size_t max{};
  std::vector<Match> arguments{};
  bool satisfied = false;
};

class Token {
public:
  explicit Token(std::string t, Requirements r = Requirements())
      : token(std::move(t)), m_requirements(std::move(r)) {}

  Token(const Token &) = default;
  Token(Token &&) noexcept = default;

  Token &operator=(const Token &) = default;
  Token &operator=(Token &&) noexcept = default;

  virtual ~Token() {
    for (auto operand : m_operands) {
      delete operand;
    }
  }

  void compile_ast() {
    ast = fmt::format("  {0}( '{1}' ) : {{", TokenTypeString[tokenType()],
                      get_token());
    for (const auto op_token : operands()) {
      ast += fmt::format("\n    {0}( '{1}' )",
                         TokenTypeString[op_token->tokenType()],
                         op_token->get_token());
    }
    ast += fmt::format("{0} }}\n", !operands().empty() ? "\n " : "");
    ast_compiled = true;
  }

  const std::string &AST() {
    if (!ast_compiled)
      compile_ast();

    return ast;
  }

  virtual const std::string &get_token() const { return token; }

  virtual Token_Type tokenType() const { return NONE; }

  Requirements &requirements() { return m_requirements; }

  const std::vector<Token *> &operands() const { return m_operands; }

  void add_operand(Token *operand) {
    ast_compiled = false;
    m_operands.emplace_back(operand);
  }

  virtual void assemble() {}

  virtual void negate() {}

protected:
  std::string token;

private:
  Requirements m_requirements;
  std::vector<Token *> m_operands{};
  std::string ast{};
  bool ast_compiled{false};
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
                                           std::size_t index) {
  std::vector<Token *> consumed;
  if (0 == min) {
    DEBUG("Trying to consume for a token which takes no operands", "");
    return consumed;
  }

  if (tokens.size() <= index + min) {
    DEBUG("Not enough tokens ({} > {})", min, tokens.size() - index - 1);
    return consumed;
  }

  const std::size_t end = index + max;
  for (std::size_t idx = index, arg_index = 0;
       idx < end && idx + 1 < tokens.size(); ++idx, ++arg_index) {
    DEBUG("Argument type required for this index is {}", arguments[arg_index]);
    if (arguments[arg_index] & tokens[idx + 1]->tokenType()) {
      DEBUG("Valid consumed token: {}, with type {}",
            tokens[idx + 1]->get_token(),
            TokenTypeString[tokens[idx + 1]->tokenType()]);
      consumed.emplace_back(tokens[idx + 1]);
    } else {
      DEBUG("Invalid consumed token: {}, with type {}",
            tokens[idx + 1]->get_token(),
            TokenTypeString[tokens[idx + 1]->tokenType()]);
      return {};
    }
  }

  satisfied = true;
  return consumed;
}

template <typename OStream> OStream &operator<<(OStream &os, const Token &t) {
  return os << t.get_token();
}
} // namespace Token
} // namespace Lexer

#endif