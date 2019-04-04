#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <memory>

#include "../Debug.hpp"

#include "../Diagnostic.hpp"
#include "Algorithm.hpp"
#include "Line.hpp"
#include "Token.hpp"
#include "Tokens.hpp"

namespace Lexer {
namespace Tokenizer {
class Tokenizer;
}
} // namespace Lexer
extern void throw_error(Lexer::Tokenizer::Tokenizer *, Diagnostics::Diagnostic);
extern void throw_warning(Lexer::Tokenizer::Tokenizer *,
                          Diagnostics::Diagnostic);

namespace {
constexpr size_t HASHED_LETTERS[] = {
    100363, 99989, 97711, 97151, 92311, 80147, 82279, 72997,
    66457,  65719, 70957, 50262, 48407, 51151, 41047, 39371,
    35401,  37039, 28697, 27791, 20201, 21523, 6449,  4813,
    16333,  13337, 3571,  5519,  26783, 71471, 68371, 104729};

constexpr char to_upper(char c) {
  return (c >= 0x61 && c <= 0x7a) ? static_cast<char>(c ^ 0x20) : c;
}

constexpr bool is_alpha(const size_t c) {
  return (c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A);
}

constexpr bool is_digit(char c) { return c >= 0x30 && c <= 0x39; }

constexpr bool is_bin_digit(char c) { return c == 0x30 || c == 0x31; }

constexpr bool is_oct_digit(char c) { return c >= 0x30 && c <= 0x37; }

constexpr bool is_hex_digit(char c) {
  return is_digit(c) || (c >= 0x61 && c <= 0x66) || (c >= 0x41 && c <= 0x46);
}

/*! Create a hash from a string.
 *
 * It should be relatively safe in regards to repeat hashes, as it reduces the
 * number of things that are valid with the two if statements. However, I'm
 * still unsure whether it's smarter to also check if they're the same thing
 * afterwards.
 *
 * @param string The string to hash
 *
 * @return The hash of the string
 */
constexpr size_t hash(const std::string_view &string) {
  if (string.length() > 8 || string.length() < 2) {
    // There are no registers, directives, or instructions which are longer than
    // 8 characters, or less than 2, so this string isn't one of them.
    return {};
  }

  const size_t first_char{static_cast<size_t>(to_upper(string.front()))};

  if (first_char != '.' && !is_alpha(first_char)) {
    // Basically, we don't really want something that's likely to be an
    // immediate value, or a label (less likely to be caught here, but may as
    // well try).
    return {};
  }

  size_t hashed{37};

  for (auto character : string) {
    const size_t as_hashed{
        HASHED_LETTERS[(static_cast<size_t>(to_upper(character)) - 0x41u) &
                       0x1F]};
    hashed = (hashed * as_hashed) ^ (first_char * as_hashed);
  }

  return hashed;
}

/*! Checks whether a string contains a valid hexadecimal value
 *  Valid hexadecimal values are of the form '0?[bB][01]+'
 *  While the LC3 only supports 16 bit values, we accept larger ones here. This
 *  is so that we can warn about the size of them later.
 */
constexpr bool is_valid_binary_literal(const std::string_view &s) {
  if (s.length() == 1) {
    // There isn't a valid binary literal with only one character (something
    // like '0', or '1' will be picked up by either the octal or decimal
    // choosers)
    return false;
  }

  if (s.length() == 2 && !is_bin_digit(s[1])) {
    // Something along the lines of 'B-' or '0B' is not valid
    return false;
  }

  if (s.length() == 3 && !is_bin_digit(s[2])) {
    // Something like '0b-' is an invalid binary value
    return false;
  }

  auto begin = s.cbegin();
  const bool negative_first = '-' == *begin;

  if (negative_first) {
    begin++;
  }

  if ('0' == *begin) {
    begin++;
  }

  if ('B' == to_upper(*begin)) {
    begin++;
  }

  if (!negative_first && '-' == *begin) {
    begin++;
  }

  return Algorithm::all(begin, s.cend(), is_bin_digit);
}

/*! Checks whether a string contains a valid hexadecimal value
 *  Valid hexadecimal values are of the form '0?[xX]-?[\da-fA-F]+'
 *  While the LC3 only supports 16 bit values, we accept larger ones here. This
 *  is so that we can warn about the size of them later.
 */
constexpr bool is_valid_hexadecimal_literal(const std::string_view &s) {
  if (s.length() == 1) {
    // A Hexadecimal value with a single character is invalid (if it's a value,
    // it'll be caught elsewhere).
    return false;
  }

  if (s.length() == 2 && !is_hex_digit(s[1])) {
    // Something along the lines of 'X-' or '0X' are not valid
    return false;
  }

  if (s.length() == 3 && !is_hex_digit(s[2])) {
    // Something like '0x-' is an invalid hex value
    return false;
  }

  auto begin = s.cbegin();
  const bool negative_first = '-' == *begin;

  if (negative_first) {
    begin++;
  }

  if ('0' == *begin) {
    begin++;
  }

  if ('X' == to_upper(*begin)) {
    begin++;
  }

  if (!negative_first && '-' == *begin) {
    begin++;
  }

  return Algorithm::all(begin, s.cend(), is_hex_digit);
}

constexpr bool is_valid_octal_literal(const std::string_view &s) {
  return Algorithm::all(s.cbegin(), s.cend(), is_oct_digit);
}

/*! Checks whether a string contains a valid decimal value
 *  Valid decimal values are of the form '((-?#?)|(#?-?))\d+'
 *  While the LC3 only supports 16 bit values, we accept larger ones here. This
 *  is so that we can warn about the size of them later.
 */
constexpr bool is_valid_decimal(const std::string_view &s) {
  if (s.length() == 1 && !is_digit(s.front())) {
    // Anything that's of length 1 that doesn't have a digit as the only
    // character is invalid
    return false;
  } else if (s.length() == 2) {
    if (('#' == s.front() || '-' == s.front()) && !is_digit(s[1])) {
      // Anything without a digit in the second position but the length is 2
      // characters is invalid.
      return false;
    }
  }

  auto begin = s.cbegin();
  const bool negative_first = '-' == *begin;

  if (negative_first) {
    begin++;
  }

  if ('#' == *begin) {
    begin++;
  }

  if (!negative_first && '-' == *begin) {
    begin++;
  }

  return Algorithm::all(begin, s.cend(), is_digit);
}

constexpr bool is_valid_label(const std::string_view &s) {
  return Algorithm::all(s.cbegin() + static_cast<size_t>(s.front() == '.'),
                        s.cend(),
                        [](auto c) { return std::isalnum(c) || c == '_'; });
}
} // namespace

namespace Lexer {
class Lexer;

namespace Tokenizer {
class Tokenizer {
public:
  Tokenizer(Lexer &t_lexer, File &t_file) : m_lexer(t_lexer), file(t_file) {}

  std::unique_ptr<Token::Token> tokenize_immediate(const std::string &s) {
    // We know this is going to be negative, as this is only ever called when
    // the next character on the line is a '-'.
    switch (to_upper(s.front())) {
    case '\'':
      break;
    case '0':
      if (s.length() > 1) {
        if ('X' == to_upper(s[1]) && is_valid_hexadecimal_literal(s)) {
          auto hex{std::make_unique<Token::Hexadecimal>(
              s, file.position().line(), file.position().column(), file.name(),
              true)};
          if (hex->is_too_big()) {
            throw_error(
                this,
                Diagnostics::Diagnostic(
                    std::make_unique<Diagnostics::DiagnosticHighlighter>(
                        file.position().column() - s.length(), s.length(),
                        file.line()),
                    fmt::format(
                        "Hexadecimal literal is too big to fit inside 16 bits"),
                    file.name(), file.position().line()));
            return std::make_unique<Token::Token>(s, file.position().line(),
                                                  file.position().column(),
                                                  file.name());
          }
          return std::move(hex);
        } else if ('B' == to_upper(s[1]) && is_valid_binary_literal(s)) {
          auto bin{std::make_unique<Token::Binary>(s, file.position().line(),
                                                   file.position().column(),
                                                   file.name(), true)};
          if (bin->is_too_big()) {
            throw_error(
                this,
                Diagnostics::Diagnostic(
                    std::make_unique<Diagnostics::DiagnosticHighlighter>(
                        file.position().column() - s.length(), s.length(),
                        file.line()),
                    fmt::format(
                        "Binary literal is too big to fit inside 16 bits"),
                    file.name(), file.position().line()));
            return std::make_unique<Token::Token>(s, file.position().line(),
                                                  file.position().column(),
                                                  file.name());
          }
          return std::move(bin);
        }

#ifdef ADDONS
        if (is_valid_octal_literal(s)) {
          auto oct{std::make_unique<Token::Octal>(s, file.position().line(),
                                                  file.position().column(),
                                                  file.name(), true)};
          if (oct->is_too_big()) {
            throw_error(
                this, Diagnostics::Diagnostic(
                          std::make_unique<Diagnostics::DiagnosticHighlighter>(
                              file.position().column() - s.length(), s.length(),
                              file.line()),
                          fmt::format(
                              "Octal literal is too big to fit inside 16 bits"),
                          file.name(), file.position().line()));
            return std::make_unique<Token::Token>(s, file.position().line(),
                                                  file.position().column(),
                                                  file.name());
          }
          return std::move(oct);
        }
#endif

        throw_error(this,
                    Diagnostics::Diagnostic(
                        std::make_unique<Diagnostics::DiagnosticHighlighter>(
                            file.position().column() - s.length(), s.length(),
                            file.line()),
                        fmt::format("Invalid immediate literal {}", s),
                        file.name(), file.position().line()));
      }
      break;
    case 'B':
      if (is_valid_binary_literal(s)) {
        auto bin{std::make_unique<Token::Binary>(s, file.position().line(),
                                                 file.position().column(),
                                                 file.name(), true)};
        if (bin->is_too_big()) {
          throw_error(
              this, Diagnostics::Diagnostic(
                        std::make_unique<Diagnostics::DiagnosticHighlighter>(
                            file.position().column() - s.length(), s.length(),
                            file.line()),
                        fmt::format(
                            "Binary literal is too big to fit inside 16 bits"),
                        file.name(), file.position().line()));
          return std::make_unique<Token::Token>(
              s, file.position().line(), file.position().column(), file.name());
        }
        return std::move(bin);
      } else {
        throw_error(this,
                    Diagnostics::Diagnostic(
                        std::make_unique<Diagnostics::DiagnosticHighlighter>(
                            file.position().column() - s.length(), s.length(),
                            file.line()),
                        "Expected a binary literal here", file.name(),
                        file.position().line()));
      }
      break;
    case 'X':
      if (is_valid_hexadecimal_literal(s)) {
        auto hex{std::make_unique<Token::Hexadecimal>(s, file.position().line(),
                                                      file.position().column(),
                                                      file.name(), true)};
        if (hex->is_too_big()) {
          throw_error(
              this, Diagnostics::Diagnostic(
                        std::make_unique<Diagnostics::DiagnosticHighlighter>(
                            file.position().column() - s.length(), s.length(),
                            file.line()),
                        "Hexadecimal literal is too big to fit inside 16 bits",
                        file.name(), file.position().line()));
          return std::make_unique<Token::Token>(
              s, file.position().line(), file.position().column(), file.name());
        }
        return std::move(hex);
      } else {
        throw_error(
            this,
            Diagnostics::Diagnostic(
                std::make_unique<Diagnostics::DiagnosticHighlighter>(
                    file.position().column() - s.length(), s.length(),
                    file.line()),
                fmt::format("Expected Hexadecimal literal, but found {}", s),
                file.name(), file.position().line()));
      }
      break;
    case '1':
      [[fallthrough]];
    case '2':
      [[fallthrough]];
    case '3':
      [[fallthrough]];
    case '4':
      [[fallthrough]];
    case '5':
      [[fallthrough]];
    case '6':
      [[fallthrough]];
    case '7':
      [[fallthrough]];
    case '8':
      [[fallthrough]];
    case '9':
      if (is_valid_decimal(s)) {
        auto decimal{std::make_unique<Token::Decimal>(s, file.position().line(),
                                                      file.position().column(),
                                                      file.name(), true)};
        if (decimal->is_too_big()) {
          throw_error(
              this, Diagnostics::Diagnostic(
                        std::make_unique<Diagnostics::DiagnosticHighlighter>(
                            file.position().column() - s.length(), s.length(),
                            file.line()),
                        fmt::format(
                            "Decimal literal is too big to fit inside 16 bits"),
                        file.name(), file.position().line()));
          return std::make_unique<Token::Token>(
              s, file.position().line(), file.position().column(), file.name());
        }
        return std::move(decimal);
      } else {
        throw_error(
            this, Diagnostics::Diagnostic(
                      std::make_unique<Diagnostics::DiagnosticHighlighter>(
                          file.position().column() - s.length(), s.length(),
                          file.line()),
                      fmt::format("Expected Decimal literal, but found {}", s),
                      file.name(), file.position().line()));
      }
      break;
    default:
      break;
    }

    return std::make_unique<Token::Token>(
        s, file.position().line(), file.position().column(), file.name());
  }

  std::unique_ptr<Token::Token> tokenize_directive(const std::string &s) {
    const auto &[column, line] = file.position();
    if (const auto _hash{hash(s)}; _hash > 0) {
      switch (_hash) {
      case ::hash(".FILL"):
        return std::make_unique<Token::Fill>(s, line, column, file.name());
      case ::hash(".STRINGZ"):
        return std::make_unique<Token::Stringz>(s, line, column, file.name());
      case ::hash(".BLKW"):
        return std::make_unique<Token::Blkw>(s, line, column, file.name());
#ifdef ADDONS
      case ::hash(".INCLUDE"):
        return std::make_unique<Token::Include>(s, line, column, file.name());
      case ::hash(".SET"):
        return std::make_unique<Token::Set>(s, line, column, file.name());
      case ::hash(".LSHIFT"):
        return std::make_unique<Token::Lshift>(s, line, column, file.name());
      case ::hash(".NEG"):
        return std::make_unique<Token::Neg>(s, line, column, file.name());
      case ::hash(".SUB"):
        return std::make_unique<Token::Sub>(s, line, column, file.name());
#endif
      case ::hash(".ORIG"):
        return std::make_unique<Token::Orig>(s, line, column, file.name());
      case ::hash(".END"):
        return std::make_unique<Token::End>(s, line, column, file.name());
      }
    }

    if (is_valid_label(s)) {
      return std::make_unique<Token::Label>(s, line, column, file.name());
    }

    return std::make_unique<Token::Token>(s, line, column, file.name());
  }

  /*! Tokenize a single word
   *
   * @param s The word to tokenize
   * @return The token the word corresponds to
   */
  std::unique_ptr<Token::Token> tokenize(const std::string &s) {
    const auto &[column, line] = file.position();
    if (const auto _hash = hash(s); _hash > 0) {
      switch (_hash) {
      case ::hash("R0"):
        [[fallthrough]];
      case ::hash("R1"):
        [[fallthrough]];
      case ::hash("R2"):
        [[fallthrough]];
      case ::hash("R3"):
        [[fallthrough]];
      case ::hash("R4"):
        [[fallthrough]];
      case ::hash("R5"):
        [[fallthrough]];
      case ::hash("R6"):
        [[fallthrough]];
      case ::hash("R7"):
        return std::make_unique<Token::Register>(s, line, column, file.name());

      case ::hash("ADD"):
        return std::make_unique<Token::Add>(s, line, column, file.name());
      case ::hash("AND"):
        return std::make_unique<Token::And>(s, line, column, file.name());
      case ::hash("LD"):
        return std::make_unique<Token::Ld>(s, line, column, file.name());
      case ::hash("NOT"):
        return std::make_unique<Token::Not>(s, line, column, file.name());
      case ::hash("RET"):
        return std::make_unique<Token::Ret>(s, line, column, file.name());
#ifdef ADDONS
      case ::hash("JMPT"):
        return std::make_unique<Token::Jmpt>(s, line, column, file.name());
#endif
      case ::hash("JMP"):
        return std::make_unique<Token::Jmp>(s, line, column, file.name());
      case ::hash("JSR"):
        return std::make_unique<Token::Jsr>(s, line, column, file.name());
      case ::hash("JSRR"):
        return std::make_unique<Token::Jsrr>(s, line, column, file.name());
      case ::hash("LEA"):
        return std::make_unique<Token::Lea>(s, line, column, file.name());
      case ::hash("LDI"):
        return std::make_unique<Token::Ldi>(s, line, column, file.name());
      case ::hash("LDR"):
        return std::make_unique<Token::Ldr>(s, line, column, file.name());
      case ::hash("ST"):
        return std::make_unique<Token::St>(s, line, column, file.name());
      case ::hash("STR"):
        return std::make_unique<Token::Str>(s, line, column, file.name());
      case ::hash("STI"):
        return std::make_unique<Token::Sti>(s, line, column, file.name());
      case ::hash("TRAP"):
        return std::make_unique<Token::Trap>(s, line, column, file.name());
      case ::hash("PUTS"):
        return std::make_unique<Token::Puts>(s, line, column, file.name());
      case ::hash("PUTSP"):
        return std::make_unique<Token::Putsp>(s, line, column, file.name());

      case ::hash("PUTC"):
        [[fallthrough]];
      case ::hash("OUT"):
        return std::make_unique<Token::Out>(s, line, column, file.name());

      case ::hash("GETC"):
        return std::make_unique<Token::Getc>(s, line, column, file.name());

      case ::hash("IN"):
        return std::make_unique<Token::In>(s, line, column, file.name());
      case ::hash("HALT"):
        return std::make_unique<Token::Halt>(s, line, column, file.name());
      case ::hash("RTI"):
        return std::make_unique<Token::Rti>(s, line, column, file.name());

      case ::hash("BR"):
        [[fallthrough]];
      case ::hash("BRNZP"):
        [[fallthrough]];
      case ::hash("BRNPZ"):
        [[fallthrough]];
      case ::hash("BRZNP"):
        [[fallthrough]];
      case ::hash("BRZPN"):
        [[fallthrough]];
      case ::hash("BRPNZ"):
        [[fallthrough]];
      case ::hash("BRPZN"):
        return std::make_unique<Token::Br>(s, true, true, true, line, column,
                                           file.name());

      case ::hash("BRN"):
        return std::make_unique<Token::Br>(s, true, false, false, line, column,
                                           file.name());
      case ::hash("BRZ"):
        return std::make_unique<Token::Br>(s, false, true, false, line, column,
                                           file.name());
      case ::hash("BRP"):
        return std::make_unique<Token::Br>(s, false, false, true, line, column,
                                           file.name());

      case ::hash("BRNZ"):
        [[fallthrough]];
      case ::hash("BRZN"):
        return std::make_unique<Token::Br>(s, true, true, false, line, column,
                                           file.name());

      case ::hash("BRNP"):
        [[fallthrough]];
      case ::hash("BRPN"):
        return std::make_unique<Token::Br>(s, true, false, true, line, column,
                                           file.name());

      case ::hash("BRZP"):
        [[fallthrough]];
      case ::hash("BRPZ"):
        return std::make_unique<Token::Br>(s, false, true, true, line, column,
                                           file.name());

      default:
        break;
      }
    }

#ifdef ADDONS
    if ('0' == s.front() && is_valid_octal_literal(s)) {
      return std::make_unique<Token::Octal>(s, line, column, file.name());
    }
#endif

    if (is_valid_decimal(s)) {
      return std::make_unique<Token::Decimal>(s, line, column, file.name());
    }

    // Due to how immediate literals are processed, this needs to occur before
    // the hex check, as otherwise it'll be treated as a valid hex digit (and
    // quite possibly seen as too big)
    if (is_valid_binary_literal(s)) {
      return std::make_unique<Token::Binary>(s, line, column, file.name());
    }

    if (is_valid_hexadecimal_literal(s)) {
      return std::make_unique<Token::Hexadecimal>(s, line, column, file.name());
    }

    if (is_valid_label(s)) {
      return std::make_unique<Token::Label>(s, line, column, file.name());
    }

    throw_error(this,
                Diagnostics::Diagnostic(
                    std::make_unique<Diagnostics::DiagnosticHighlighter>(
                        column, s.length(), file.line()),
                    fmt::format("Invalid token: {}", s), file.name(), line));
    return std::make_unique<Token::Token>(s, line, column, file.name());
  }

  auto extraneous(char character) -> void {
    throw_warning(this,
                  Diagnostics::Diagnostic(
                      std::make_unique<Diagnostics::DiagnosticHighlighter>(
                          file.position().column(), 0, file.line()),
                      fmt::format("Extraneous '{}' found.", character),
                      file.name(), file.position().line()));
  }

  std::vector<std::unique_ptr<Token::Token>> tokenize_line(Line line) {
    std::vector<std::unique_ptr<Token::Token>> l_tokens{};
    char terminator{};

    while (!line.at_end()) {
      line.skip_while([](auto &&c) -> bool { return std::isspace(c); });

      const auto token_start{line.index()};
      const auto token_end{line.find_if([](auto &&c) -> bool {
        return !(std::isalnum(c) || '_' == c || '-' == c);
      })};

      file.set_column(token_start);

      if (auto &&token{line.substr(token_start, token_end)};
          0 == token.size()) {
        switch (const auto next{line.next()}; next) {
        case '.': {
          terminator = '\0';
          // Now we tokenize a directive
          // const auto dir_start = line.index();
          token = line.substr(token_end, line.find_if([](auto &&c) -> bool {
            return !(std::isalnum(c));
          }));
          l_tokens.emplace_back(tokenize_directive(token));
          break;
        }
        case '-': { // Negative immediate (hopefully)
          terminator = '\0';
          token = line.substr(line.index(), line.find_if([](auto &&c) -> bool {
            return !(std::isalnum(c));
          }));
          if (token.size() > 0) {
            auto &&tok{tokenize_immediate(token)};
            l_tokens.emplace_back(std::move(tok));
          } else {
            throw_error(
                this, Diagnostics::Diagnostic(
                          std::make_unique<Diagnostics::DiagnosticHighlighter>(
                              file.position().column(), 0, file.line()),
                          "Extraneous '-' found.", file.name(),
                          file.position().line()));
          }
          break;
        }
        case ',': // Just a seperator between operands, not required.
          [[fallthrough]];
        case ':': // As above.
          if (',' == terminator || ':' == terminator) {
            extraneous(next);
          } else if (':' == next) {
            if (l_tokens.size() > 0 &&
                l_tokens.back()->token_type() != TokenType::LABEL) {
              extraneous(next);
            }
          } else if (',' == next) {
            if (l_tokens.size() == 0 ||
                (l_tokens.back()->token_type() != TokenType::REGISTER &&
                 l_tokens.back()->token_type() != TokenType::IMMEDIATE)) {
              extraneous(next);
            }
          }
          terminator = next;
          break;
        case '"': // string
          [[fallthrough]];
        case '\'': { // character
          terminator = '\0';
          line.ignore(Line::ESCAPE_SEQUENCE);
          const auto begin{line.index()};
          const auto end{line.find_next(next)};
          line.ignore(Line::RESET);
          if (-1u == end) {
#ifdef ADDONS
            const std::string unterminated_litera{
                next == '"' ? "Unterminated String literal"
                            : "Unterminated Character literal"};
#else
            static const std::string unterminated_litera(
                "Unterminated String literal");
#endif
            throw_error(
                this,
                Diagnostics::Diagnostic(
                    std::make_unique<Diagnostics::DiagnosticHighlighter>(
                        token_start, line.index() - begin + 1, file.line()),
                    unterminated_litera, file.name(), file.position().line()));
            l_tokens.emplace_back(std::make_unique<Token::Token>(
                unterminated_litera, file.position().line(), token_start,
                file.name()));
          } else if ('"' == next) {
            auto &&str{line.substr(begin, end)};
            l_tokens.emplace_back(std::make_unique<Token::String>(
                str, file.position().line(), token_start, file.name()));
          } else if ('\'' == next) {
            auto &&character{line.substr(begin, end)};
#ifdef ADDONS
            if (character.size() > 1 && '\\' != character.front()) {
              throw_error(
                  this,
                  Diagnostics::Diagnostic(
                      std::make_unique<Diagnostics::DiagnosticHighlighter>(
                          token_start, character.length(), file.line()),
                      fmt::format("Invalid character literal '{}'", character),
                      file.name(), file.position().line()));
            } else {
              l_tokens.emplace_back(std::make_unique<Token::Character>(
                  character, file.position().line(), file.position().column(),
                  file.name()));
            }
#else
            throw_error(
                this, Diagnostics::Diagnostic(
                          std::make_unique<Diagnostics::DiagnosticHighlighter>(
                              begin - 1, character.length() + 1, file.line()),
                          "Found character literal, but Addons aren't "
                          "enabled",
                          file.name(), file.position().line()));
#endif
          }
          break;
        }
        case '/':
          // Both mean a comment (though, technically, a '//' is a
          // comment, but '/' is not used for anything.)
          file.set_column(line.index() - 1);
          if ('/' != line.next()) {
            throw_warning(
                this, Diagnostics::Diagnostic(
                          std::make_unique<Diagnostics::DiagnosticHighlighter>(
                              file.position().column(), 0, file.line()),
                          "Found '/', acting as if it's supposed to be '//'",
                          file.name(), file.position().line()));
          }
          [[fallthrough]];
        case ';':
#ifdef KEEP_COMMENTS
          l_tokens.emplace_back(std::make_unique<Token::Comment>(
              line.substr(line.index(), -1), file.position().line(),
              file.position().column(), file.name()));
#endif
          line.skip_while([](auto &&) -> bool { return true; });
          break;
        case '#': {
          token = line.substr(token_start, line.find_if([](auto c) {
            return !(is_digit(c) || '-' == c);
          }));
          if (is_valid_decimal(token)) {
            auto &&decimal{std::make_unique<Token::Decimal>(
                token, file.position().line(), file.position().column(),
                file.name())};
            if (decimal->is_too_big()) {
              throw_error(
                  this,
                  Diagnostics::Diagnostic(
                      std::make_unique<Diagnostics::DiagnosticHighlighter>(
                          token_start, decimal->get_token().length(),
                          file.line()),
                      fmt::format("Immediate literal requires more than 16 "
                                  "bits to represent"),
                      file.name(), file.position().line()));
              l_tokens.emplace_back(std::make_unique<Token::Token>(
                  token, file.position().line(), file.position().column(),
                  file.name()));
            } else {
              l_tokens.emplace_back(std::move(decimal));
            }
          } else {
            l_tokens.emplace_back(std::make_unique<Token::Token>(
                token, file.position().line(), file.position().column(),
                file.name()));
          }
          break;
        }
        default:
          terminator = '\0';
          break;
        }
      } else {
        if (auto &&t_token{tokenize(token)};
            t_token->token_type() == TokenType::IMMEDIATE &&
            t_token->is_too_big()) {
          throw_error(
              this,
              Diagnostics::Diagnostic(
                  std::make_unique<Diagnostics::DiagnosticHighlighter>(
                      token_start, t_token->get_token().length(), file.line()),
                  fmt::format("Immediate literal requires more than 16 "
                              "bits to represent"),
                  file.name(), file.position().line()));
          l_tokens.emplace_back(std::make_unique<Token::Token>(
              token, file.position().line(), file.position().column(),
              file.name()));
        } else {
          l_tokens.emplace_back(std::move(t_token));
        }
        terminator = '\0';
      }
    }

    return l_tokens;
  }

  Lexer &lexer() const { return m_lexer; }

private:
  Lexer &m_lexer;
  File &file;
}; // namespace Tokenizer
} // namespace Tokenizer
} // namespace Lexer

#endif
