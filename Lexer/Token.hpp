#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cassert>
#include <map>
#include <string>
#include <vector>

#include "../Algorithm.hpp"
#include "../Debug.hpp"
#include "../Diagnostic.hpp"
#include "../Notifier.hpp"
#include "File.hpp"
#include "fmt/format.h"

using word = int;
constexpr word operator""_word(unsigned long long val) {
  return static_cast<word>(val);
}

constexpr word operator""_words(unsigned long long val) {
  return static_cast<word>(val);
}

namespace Lexer {

struct Symbol {
  Symbol() = default;
  Symbol(const std::string &tName, size_t tAddress, const std::string &tFile,
         size_t tColumn, size_t tLine)
      : mName(tName), mAddress(tAddress), mFile(tFile), mColumn(tColumn),
        mLine(tLine) {}

  Symbol(const Symbol &) = default;
  Symbol(Symbol &&) noexcept = default;

  Symbol &operator=(const Symbol &) = default;
  Symbol &operator=(Symbol &&) noexcept = default;

  ~Symbol() = default;

  const auto &name() const { return mName; }
  const auto &file() const { return mFile; }

  auto address() const { return mAddress; }
  auto column() const { return mColumn; }
  auto line() const { return mLine; }

  std::string mName;
  size_t mAddress;
  std::string mFile;
  size_t mColumn;
  size_t mLine;
};

enum TokenType {
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
  JMPT,
#endif
#ifdef KEEP_COMMENTS
  COMMENT,
#endif
};

const char *TokenTypeString[] = {
    "INVALID",
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
    "Directive .SUB",
    "Instruction JMPT",
#endif
#ifdef KEEP_COMMENTS
    "Comment",
#endif
};

template <typename OStream>
inline OStream &operator<<(OStream &os, TokenType type) {
  return os << TokenTypeString[type];
}

class Match {
public:
  Match() = default;
  explicit Match(TokenType t) : matches({t}) {}

  Match(const Match &) = default;
  Match(Match &&) noexcept = default;

  Match &operator=(const Match &) = default;
  Match &operator=(Match &&) noexcept = default;

  ~Match() = default;

  template <typename OStream>
  friend OStream &operator<<(OStream &os, const Match &t) {
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

                os << token_type;

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
  Match &operator|(TokenType t) {
    matches.push_back(t);
    return *this;
  }

  bool operator&(const Match &t) const {
    for (auto &&match : t.matches) {
      if (*this & match) {
        return true;
      }
    }

    return false;
  }

  bool operator&(TokenType t) const {
    for (auto &&match : matches) {
      if (match == t) {
        return true;
      }
    }

    return false;
  }

private:
  std::vector<TokenType> matches{};
};

namespace Token {
class Token;

class Requirements {
public:
  Requirements() = default;
  explicit Requirements(size_t _min, std::vector<Match> operands = {},
                        size_t _max = 0)
      : min(_min), max(_max), arguments(std::move(operands)) {
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

  const auto &argumentRequirements() const { return arguments; }

  std::vector<std::unique_ptr<Token>>
  consume(std::vector<std::unique_ptr<Token>> &tokens, std::size_t index,
          const File &file) const;

  bool are_satisfied() const { return satisfied; }

private:
  size_t min{};
  size_t max{};
  std::vector<Match> arguments{};
  mutable bool satisfied{false};
};

class AssembledToken {
public:
  AssembledToken() = default;
  AssembledToken(uint16_t tBin, std::string tLSTStr)
      : mBin{tBin}, mLSTStr{std::move(tLSTStr)} {}
  AssembledToken(const AssembledToken &) = default;
  AssembledToken(AssembledToken &&) noexcept = default;
  AssembledToken &operator=(const AssembledToken &) = default;
  AssembledToken &operator=(AssembledToken &&) noexcept = default;
  ~AssembledToken() = default;

  const auto &lstStr() const { return mLSTStr; }
  auto binary() const { return mBin; }

private:
  uint16_t mBin;
  std::string mLSTStr;
};

class Token {
public:
  Token(std::string t, size_t tLine, size_t tColumn, const std::string &tFile,
        Requirements r = Requirements())
      : token(std::move(t)), atLine(tLine), atColumn(tColumn), mFile(tFile),
        mRequirements(std::move(r)) {}

  Token(const Token &) = default;
  Token(Token &&) noexcept = default;

  Token &operator=(const Token &) = default;
  Token &operator=(Token &&) noexcept = default;

  virtual ~Token() = default;

  const auto &operands() const { return mOperands; }

  void compileAST() {
    ast = fmt::format("  {} ( '{}' ) [ File: {}, Line: {}, Column: {} ] {{",
                      tokenType(), getToken(), file(), line(), column());
    for (const auto &operand : operands()) {
      if (operand->tokenType() != STRING
#ifdef ADDONS
          && '\'' != operand->getToken().front()
#endif
      ) {
        ast +=
            fmt::format("\n    {} ( '{}' ) [ File: {}, Line: {}, Column: {} ]",
                        operand->tokenType(), *operand, operand->file(),
                        operand->line(), operand->column());
      } else {
        ast += fmt::format("\n    {} ( {} ) [ File: {}, Line: {}, Column: {} ]",
                           operand->tokenType(), *operand, operand->file(),
                           operand->line(), operand->column());
      }
    }
    ast += fmt::format("{0} }}\n", !operands().empty() ? "\n " : std::string{});
    astCompiled = true;
  }

  const std::string &AST() {
    if (!astCompiled)
      compileAST();

    return ast;
  }

  virtual const std::string &getToken() const { return token; }

  virtual TokenType tokenType() const { return NONE; }

  auto &requirements() const { return mRequirements; }

  void addOperand(std::unique_ptr<Token> operand) {
    astCompiled = false;
    mOperands.emplace_back(std::move(operand));
  }

  virtual void assemble(int16_t &programCounter, size_t width,
                        const std::map<std::string, Symbol> &symbol) {}
  virtual word memoryRequired() const { return -1; }

  virtual bool isTooBig() const { return tooBig; }

  const std::string &file() const { return mFile; }
  size_t line() const { return atLine; }
  size_t column() const { return atColumn; }

  void setAssembled(AssembledToken assembled) {
    asAssembled = {std::move(assembled)};
  }
  auto &&assembled() const { return asAssembled; }

protected:
  std::string token;
  bool tooBig{false};
  std::vector<AssembledToken> asAssembled{};

private:
  size_t atLine;
  size_t atColumn;
  std::string mFile;
  Requirements mRequirements;
  std::vector<std::unique_ptr<Token>> mOperands{};
  std::string ast{};
  bool astCompiled{false};
};

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
    // DEBUG("Trying to consume for a token which takes no operands", "");
    satisfied = true;
    return consumed;
  }

  if (tokens.size() <= index + min) {
    // DEBUG("Not enough tokens ({} > {})", min, tokens.size() - index - 1);
    return consumed;
  }

  if (max == -1u) {
    // Need to match at least one of the tokens
    if (!(arguments[0] & tokens[index + 1]->tokenType())) {
      auto &&token = tokens[index + 1];
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(
              token->column(), token->getToken().length(),
              file.line(token->line() - 1)),
          fmt::format("Expected {}, but found '{}' (with type {})",
                      arguments[0], token->getToken(), token->tokenType()),
          token->file(), token->line());
      return consumed;
    }

    consumed.emplace_back(std::move(tokens[index + 1]));

    size_t idx = index + 2;
    while (idx < tokens.size() && arguments[0] & tokens[idx]->tokenType()) {
      // DEBUG("Found token {}", tokens[idx]->getToken());
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
    if (arguments[arg_index] & tokens[idx + 1]->tokenType()) {
      // DEBUG("Valid consumed token: {}, with type {}", token->getToken(),
      //      token->tokenType());
      consumed.emplace_back(std::move(tokens[idx + 1]));
    } else if (arg_index >= min && arg_index <= max) {
      // DEBUG("Valid consumption, finished with argument count {}", arg_index);
      break;
    } else {
      // DEBUG("Invalid consumed token: {}, with type {}", token->getToken(),
      // token->tokenType());
      Notification::error_notifications << Diagnostics::Diagnostic(
          std::make_unique<Diagnostics::DiagnosticHighlighter>(
              token->column(), token->getToken().length(),
              file.line(token->line() - 1)),
          fmt::format("Expected {}, but found '{}' (with type {})",
                      arguments[arg_index], token->getToken(),
                      token->tokenType()),
          token->file(), token->line());
      return consumed;
    }
  }

  satisfied = true;
  return consumed;
}

template <typename OStream>
inline OStream &operator<<(OStream &os, const Token &t) {
  return os << t.getToken();
}
} // namespace Token
} // namespace Lexer

#endif