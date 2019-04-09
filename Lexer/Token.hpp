#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cassert>
#include <map>
#include <string>
#include <utility>
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
  Symbol(std::string t_name, size_t t_address, std::string t_file,
         size_t t_column, size_t t_line)
      : m_name(std::move(t_name)), m_address(t_address),
        m_file(std::move(t_file)), m_column(t_column), m_line(t_line) {}

  Symbol(const Symbol &) = default;
  Symbol(Symbol &&) = default;

  Symbol &operator=(const Symbol &) = default;
  Symbol &operator=(Symbol &&) = default;

  ~Symbol() = default;

  constexpr const auto &name() const { return m_name; }
  constexpr const auto &file() const { return m_file; }

  constexpr auto address() const { return m_address; }
  constexpr auto column() const { return m_column; }
  constexpr auto line() const { return m_line; }

  const std::string m_name;
  const size_t m_address{};
  const std::string m_file{};
  const size_t m_column{};
  const size_t m_line{};
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

namespace {
const char *TOKEN_TYPE_STRING[] = {
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
} // namespace

template <typename OStream>
inline OStream &operator<<(OStream &os, TokenType type) {
  return os << TOKEN_TYPE_STRING[type];
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

namespace Token {
class Token;

class Requirements {
public:
  Requirements() = default;
  explicit Requirements(size_t _min, std::vector<Match> operands = {},
                        size_t _max = 0)
      : min(_min), max(_min > _max ? _min : _max),
        arguments(std::move(operands)) {

    assert(arguments.size() >= min && arguments.size() <= max);
  }

  Requirements(const Requirements &) = default;
  Requirements(Requirements &&) = default;

  Requirements &operator=(const Requirements &) = default;
  Requirements &operator=(Requirements &&) = default;

  ~Requirements() = default;

  auto count() const { return min; }

  const auto &argument_requirements() const { return arguments; }

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

class AssembledToken {
public:
  AssembledToken() = default;
  AssembledToken(uint16_t t_bin, const std::string &t_lst_str)
      : m_bin{t_bin}, m_lst_str{std::move(t_lst_str)} {}
  AssembledToken(const AssembledToken &) = default;
  AssembledToken(AssembledToken &&) noexcept = default;
  AssembledToken &operator=(const AssembledToken &) = default;
  AssembledToken &operator=(AssembledToken &&) noexcept = default;
  ~AssembledToken() = default;

  const auto &lstStr() const { return m_lst_str; }
  auto binary() const {
    return std::make_pair(static_cast<char>((m_bin >> 8) & 0xFF),
                          static_cast<char>(m_bin & 0xFF));
  }

private:
  uint16_t m_bin{};
  std::string m_lst_str;
};

class Token {
public:
  Token(const std::string &t, size_t t_line, size_t t_column,
        std::string t_file, Requirements r = Requirements())
      : token(std::move(t)), at_line(t_line), at_column(t_column),
        m_file(std::move(t_file)), m_requirements(std::move(r)) {}

  Token(const Token &) = default;
  Token(Token &&) = default;

  Token &operator=(const Token &) = default;
  Token &operator=(Token &&) = default;

  virtual ~Token() = default;

  const auto &operands() const { return m_operands; }

  void compile_ast() {
    ast = fmt::format("  {} ( '{}' ) [ File: {}, Line: {}, Column: {} ] {{",
                      token_type(), get_token(), file(), line(), column());
    for (const auto &operand : operands()) {
      if (operand->token_type() != STRING
#ifdef ADDONS
          && '\'' != operand->get_token().front()
#endif
      ) {
        ast +=
            fmt::format("\n    {} ( '{}' ) [ File: {}, Line: {}, Column: {} ]",
                        operand->token_type(), *operand, operand->file(),
                        operand->line(), operand->column());
      } else {
        ast += fmt::format("\n    {} ( {} ) [ File: {}, Line: {}, Column: {} ]",
                           operand->token_type(), *operand, operand->file(),
                           operand->line(), operand->column());
      }
    }
    ast += fmt::format("{0} }}\n", !operands().empty() ? "\n " : std::string{});
    ast_compiled = true;
  }

  const std::string &AST() {
    if (!ast_compiled)
      compile_ast();

    return ast;
  }

  virtual const std::string &get_token() const { return token; }

  virtual TokenType token_type() const { return NONE; }

  auto &requirements() const { return m_requirements; }

  void add_operand(std::unique_ptr<Token> operand) {
    ast_compiled = false;
    m_operands.emplace_back(std::move(operand));
  }

  virtual void assemble(int16_t &programCounter, size_t width,
                        const std::map<std::string, Symbol> &symbol) {}
  virtual word memory_required() const { return -1; }

  virtual bool is_too_big() const { return too_big; }

  const std::string &file() const { return m_file; }
  size_t line() const { return at_line; }
  size_t column() const { return at_column; }

  void set_assembled(const AssembledToken &assembled) {
    as_assembled = {std::move(assembled)};
  }
  auto &&assembled() const { return as_assembled; }

protected:
  std::string token;
  bool too_big{false};
  std::vector<AssembledToken> as_assembled{};

private:
  const size_t at_line;
  const size_t at_column;
  const std::string m_file;
  Requirements m_requirements;
  std::vector<std::unique_ptr<Token>> m_operands{};
  std::string ast{};
  bool ast_compiled{false};
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

template <typename OStream>
inline OStream &operator<<(OStream &os, const Token &t) {
  return os << t.get_token();
}
} // namespace Token
} // namespace Lexer

#endif