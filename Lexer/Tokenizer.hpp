#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <memory>

#include "../Debug.hpp"

#include "../Diagnostic.hpp"
#include "Line.hpp"
#include "Token.hpp"
#include "Tokens.hpp"

namespace Lexer {
namespace Tokenizer {
class Tokenizer;
}
} // namespace Lexer
extern void throwError(Lexer::Tokenizer::Tokenizer *, Diagnostics::Diagnostic);
extern void throwWarning(Lexer::Tokenizer::Tokenizer *,
                         Diagnostics::Diagnostic);

static constexpr size_t hashedLetters[] = {
    100363, 99989, 97711, 97151, 92311, 80147, 82279, 72997,
    66457,  65719, 70957, 50262, 48407, 51151, 41047, 39371,
    35401,  37039, 28697, 27791, 20201, 21523, 6449,  4813,
    16333,  13337, 3571,  5519,  26783, 71471, 68371, 104729};

static inline constexpr char toUpper(char c) {
  return (c >= 0x61 && c <= 0x7a) ? static_cast<char>(c ^ 0x20) : c;
}

static inline constexpr bool isAlpha(const size_t c) {
  return (c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A);
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
static constexpr size_t hash(const std::string_view &string) {
  if (string.length() > 8 || string.length() < 2) {
    // There are no registers, directives, or instructions which are longer than
    // 8 characters, or less than 2, so this string isn't one of them.
    return {};
  }

  if (const size_t firstCharacter{static_cast<size_t>(toUpper(string.front()))};
      firstCharacter != '.' && !isAlpha(firstCharacter)) {
    // Basically, we don't really want something that's likely to be an
    // immediate value, or a label (less likely to be caught here, but may as
    // well try).
    return {};
  } else {
    size_t hashed{37};

    for (const char character : string) {
      const size_t as_hashed{
          hashedLetters[(static_cast<size_t>(toUpper(character)) - 0x41u) &
                        0x1F]};
      hashed = (hashed * as_hashed) ^ (firstCharacter * as_hashed);
    }

    return hashed;
  }
}

static bool isValidBinaryLiteral(const std::string_view &s) {
  if (s.size() == 1) {
    return false;
  }

  if ('B' == toUpper(s.front())) {
    return std::all_of(s.cbegin() + 1, s.cend(),
                       [](auto &&c) { return '0' == c || '1' == c; });
  }

  if (s.size() > 2 && 'B' == toUpper(s[1])) {
    return std::all_of(s.cbegin() + 2, s.cend(),
                       [](auto &&c) { return '0' == c || '1' == c; });
  }

  return false;
}

static bool isValidHexadecimalLiteral(const std::string_view &s) {
  if (s.length() == 1) {
    return false;
  }

  if ('X' == toUpper(s.front())) {
    if ('-' == s[1]) {
      if (s.length() == 2) {
        return false;
      }
      return std::all_of(s.cbegin() + 2, s.cend(), ::isxdigit);
    }
    return std::all_of(s.cbegin() + 1, s.cend(), ::isxdigit);
  }

  if (s.size() > 2 && 'X' == toUpper(s[1])) {
    if ('-' == s[2]) {
      if (s.length() == 3) {
        return false;
      }
      return std::all_of(s.cbegin() + 3, s.cend(), ::isxdigit);
    }
    return std::all_of(s.cbegin() + 2, s.cend(), ::isxdigit);
  }

  return false;
}

static inline bool isValidOctalLiteral(const std::string_view &s) {
  return std::all_of(s.cbegin(), s.cend(),
                     [](auto &&c) { return c <= 0x37 && c >= 0x30; });
}

static inline bool isValidDecimalLiteral(const std::string_view &s) {
  size_t offset{0};
  if ('#' == s.front()) {
    if (s.length() < 2) {
      return false;
    } else if (s[1] == '-') {
      if (s.length() < 3) {
        return false;
      }
      offset = 2;
    } else {
      offset = 1;
    }
  }
  return std::all_of(s.cbegin() + offset, s.cend(), ::isdigit);
}

static bool isValidLabel(const std::string_view &s) {
  return std::all_of('.' == s.front() ? s.cbegin() + 1 : s.cbegin(), s.cend(),
                     [](auto &&c) { return std::isalnum(c) || '_' == c; });
}

namespace Lexer {
class Lexer;

namespace Tokenizer {
class Tokenizer {
public:
  Tokenizer(Lexer &t_lexer, File &t_file) : m_lexer(t_lexer), file(t_file) {}

  std::unique_ptr<Token::Token> tokenizeImmediate(const std::string &s) {
    // We know this is going to be negative, as this is only ever called when
    // the next character on the line is a '-'.
    switch (toUpper(s.front())) {
    case '\'':
      break;
    case '0':
      if (s.size() > 1) {
        if ('X' == toUpper(s[1]) && isValidHexadecimalLiteral(s)) {
          auto hex{std::make_unique<Token::Hexadecimal>(
              s, file.position().line(), file.position().column(), file.name(),
              true)};
          if (hex->isTooBig()) {
            throwError(
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
        } else if ('B' == toUpper(s[1]) && isValidBinaryLiteral(s)) {
          auto bin{std::make_unique<Token::Binary>(s, file.position().line(),
                                                   file.position().column(),
                                                   file.name(), true)};
          if (bin->isTooBig()) {
            throwError(
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
        if (isValidOctalLiteral(s)) {
          auto oct{std::make_unique<Token::Octal>(s, file.position().line(),
                                                  file.position().column(),
                                                  file.name(), true)};
          if (oct->isTooBig()) {
            throwError(
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
        throwError(this,
                   Diagnostics::Diagnostic(
                       std::make_unique<Diagnostics::DiagnosticHighlighter>(
                           file.position().column() - s.length(), s.length(),
                           file.line()),
                       fmt::format("Invalid immediate literal {}", s),
                       file.name(), file.position().line()));
      }
      break;
    case 'B':
      if (isValidBinaryLiteral(s)) {
        auto bin{std::make_unique<Token::Binary>(s, file.position().line(),
                                                 file.position().column(),
                                                 file.name(), true)};
        if (bin->isTooBig()) {
          throwError(this,
                     Diagnostics::Diagnostic(
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
        // DEBUG("Expected Binary literal, but found {}", s);
        throwError(this,
                   Diagnostics::Diagnostic(
                       std::make_unique<Diagnostics::DiagnosticHighlighter>(
                           file.position().column() - s.length(), s.length(),
                           file.line()),
                       "Expected a binary literal here", file.name(),
                       file.position().line()));
      }
      break;
    case 'X':
      DEBUG("Found potential hex value {}", s);
      if (isValidHexadecimalLiteral(s)) {
        auto hex{std::make_unique<Token::Hexadecimal>(s, file.position().line(),
                                                      file.position().column(),
                                                      file.name(), true)};
        if (hex->isTooBig()) {
          throwError(
              this,
              Diagnostics::Diagnostic(
                  std::make_unique<Diagnostics::DiagnosticHighlighter>(
                      file.position().column() - s.length(), s.length(),
                      file.line()),
                  fmt::format(
                      "Hexadecimal literal is too big to fit inside 16 bits"),
                  file.name(), file.position().line()));
          return std::make_unique<Token::Token>(
              s, file.position().line(), file.position().column(), file.name());
        }
        return std::move(hex);
      } else {
        throwError(
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
      if (isValidDecimalLiteral(s)) {
        auto decimal{std::make_unique<Token::Decimal>(s, file.position().line(),
                                                      file.position().column(),
                                                      file.name(), true)};
        if (decimal->isTooBig()) {
          throwError(
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
        throwError(this,
                   Diagnostics::Diagnostic(
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

  std::unique_ptr<Token::Token> tokenizeDirective(const std::string &s) {
    if (const auto _hash{hash(s)}; _hash > 0) {
      switch (_hash) {
#ifdef ADDONS
      case ::hash(".INCLUDE"):
        return std::make_unique<Token::Include>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash(".SET"):
        return std::make_unique<Token::Set>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash(".LSHIFT"):
        return std::make_unique<Token::Lshift>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash(".NEG"):
        return std::make_unique<Token::Neg>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash(".SUB"):
        return std::make_unique<Token::Sub>(
            s, file.position().line(), file.position().column(), file.name());
#endif
      case ::hash(".ORIG"):
        return std::make_unique<Token::Orig>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash(".END"):
        return std::make_unique<Token::End>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash(".STRINGZ"):
        return std::make_unique<Token::Stringz>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash(".FILL"):
        return std::make_unique<Token::Fill>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash(".BLKW"):
        return std::make_unique<Token::Blkw>(
            s, file.position().line(), file.position().column(), file.name());
      }
    }

    if (isValidLabel(s)) {
      return std::make_unique<Token::Label>(
          s, file.position().line(), file.position().column(), file.name());
    }

    return std::make_unique<Token::Token>(
        s, file.position().line(), file.position().column(), file.name());
  }

  /*! Tokenize a single word
   *
   * @param s The word to tokenize
   * @return The token the word corresponds to
   */
  std::unique_ptr<Token::Token> tokenize(const std::string &s) {
    if (const auto _hash = hash(s); _hash > 0) {
      switch (_hash) {
      case ::hash("ADD"):
        return std::make_unique<Token::Add>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("AND"):
        return std::make_unique<Token::And>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("NOT"):
        return std::make_unique<Token::Not>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("RET"):
        return std::make_unique<Token::Ret>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("JMP"):
        return std::make_unique<Token::Jmp>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("JSR"):
        return std::make_unique<Token::Jsr>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("JSRR"):
        return std::make_unique<Token::Jsrr>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("LD"):
        return std::make_unique<Token::Ld>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("LEA"):
        return std::make_unique<Token::Lea>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("LDI"):
        return std::make_unique<Token::Ldi>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("LDR"):
        return std::make_unique<Token::Ldr>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("ST"):
        return std::make_unique<Token::St>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("STR"):
        return std::make_unique<Token::Str>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("STI"):
        return std::make_unique<Token::Sti>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("TRAP"):
        return std::make_unique<Token::Trap>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("PUTS"):
        return std::make_unique<Token::Puts>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("PUTSP"):
        return std::make_unique<Token::Putsp>(
            s, file.position().line(), file.position().column(), file.name());

      case ::hash("PUTC"):
        [[fallthrough]];
      case ::hash("OUT"):
        return std::make_unique<Token::Out>(
            s, file.position().line(), file.position().column(), file.name());

      case ::hash("GETC"):
        return std::make_unique<Token::Getc>(
            s, file.position().line(), file.position().column(), file.name());

      case ::hash("IN"):
        return std::make_unique<Token::In>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("HALT"):
        return std::make_unique<Token::Halt>(
            s, file.position().line(), file.position().column(), file.name());
      case ::hash("RTI"):
        return std::make_unique<Token::Rti>(
            s, file.position().line(), file.position().column(), file.name());

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
        return std::make_unique<Token::Br>(
            s, true, true, true, file.position().line(),
            file.position().column(), file.name());

      case ::hash("BRN"):
        return std::make_unique<Token::Br>(
            s, true, false, false, file.position().line(),
            file.position().column(), file.name());
      case ::hash("BRZ"):
        return std::make_unique<Token::Br>(
            s, false, true, false, file.position().line(),
            file.position().column(), file.name());
      case ::hash("BRP"):
        return std::make_unique<Token::Br>(
            s, false, false, true, file.position().line(),
            file.position().column(), file.name());

      case ::hash("BRNZ"):
        [[fallthrough]];
      case ::hash("BRZN"):
        return std::make_unique<Token::Br>(
            s, true, true, false, file.position().line(),
            file.position().column(), file.name());

      case ::hash("BRNP"):
        [[fallthrough]];
      case ::hash("BRPN"):
        return std::make_unique<Token::Br>(
            s, true, false, true, file.position().line(),
            file.position().column(), file.name());

      case ::hash("BRZP"):
        [[fallthrough]];
      case ::hash("BRPZ"):
        return std::make_unique<Token::Br>(
            s, false, true, true, file.position().line(),
            file.position().column(), file.name());

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
        return std::make_unique<Token::Register>(
            s, file.position().line(), file.position().column(), file.name());
      default:
        break;
      }
    }

#ifdef ADDONS
    if (isValidOctalLiteral(s)) {
      return std::make_unique<Token::Octal>(
          s, file.position().line(), file.position().column(), file.name());
    }
#endif

    if (isValidDecimalLiteral(s)) {
      return std::make_unique<Token::Decimal>(
          s, file.position().line(), file.position().column(), file.name());
    }

    if (isValidHexadecimalLiteral(s)) {
      return std::make_unique<Token::Hexadecimal>(
          s, file.position().line(), file.position().column(), file.name());
    }

    if (isValidBinaryLiteral(s)) {
      return std::make_unique<Token::Binary>(
          s, file.position().line(), file.position().column(), file.name());
    }

    if (isValidLabel(s)) {
      return std::make_unique<Token::Label>(
          s, file.position().line(), file.position().column(), file.name());
    }

    throwError(this, Diagnostics::Diagnostic(
                         std::make_unique<Diagnostics::DiagnosticHighlighter>(
                             file.position().column() - s.length(), s.length(),
                             file.line()),
                         fmt::format("Invalid token: {}", s), file.name(),
                         file.position().line()));
    return std::make_unique<Token::Token>(
        s, file.position().line(), file.position().column(), file.name());
  }

  auto extraneous(char character) -> void {
    throwWarning(this, Diagnostics::Diagnostic(
                           std::make_unique<Diagnostics::DiagnosticHighlighter>(
                               file.position().column(), 0, file.line()),
                           fmt::format("Extraneous '{}' found.", character),
                           file.name(), file.position().line()));
  }

  std::vector<std::unique_ptr<Token::Token>> tokenizeLine(Line line) {
    std::vector<std::unique_ptr<Token::Token>> lTokens{};
    char terminator{};

    while (!line.atEnd()) {
      line.skip_while([](auto &&c) -> bool { return std::isspace(c); });

      const auto token_start{line.index()};
      const auto token_end{line.find_if([](auto &&c) -> bool {
        return !(std::isalnum(c) || '_' == c || '-' == c);
      })};

      file.setColumn(token_start);

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
          lTokens.emplace_back(tokenizeDirective(token));
          break;
        }
        case '-': { // Negative immediate (hopefully)
          terminator = '\0';
          token = line.substr(line.index(), line.find_if([](auto &&c) -> bool {
            return !(std::isalnum(c));
          }));
          if (token.size() > 0) {
            auto &&t{tokenizeImmediate(token)};
            lTokens.emplace_back(std::move(t));
          } else {
            throwError(this,
                       Diagnostics::Diagnostic(
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
            if (lTokens.size() > 0 &&
                lTokens.back()->tokenType() != TokenType::LABEL) {
              extraneous(next);
            }
          } else if (',' == next) {
            if (lTokens.size() == 0 ||
                (lTokens.back()->tokenType() != TokenType::REGISTER &&
                 lTokens.back()->tokenType() != TokenType::IMMEDIATE)) {
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
            const std::string unterminatedLiteral{
                fmt::format("Unterminated {} literal",
                            next == '"' ? "String" : "Character")};
#else
            const std::string unterminatedLiteral(
                "Unterminated String literal");
#endif
            throwError(this,
                       Diagnostics::Diagnostic(
                           std::make_unique<Diagnostics::DiagnosticHighlighter>(
                               token_start, line.index() - begin, file.line()),
                           unterminatedLiteral, file.name(),
                           file.position().line()));
            lTokens.emplace_back(std::make_unique<Token::Token>(
                unterminatedLiteral, file.position().line(), token_start,
                file.name()));
          } else if ('"' == next) {
            auto &&str{line.substr(begin, end)};
            lTokens.emplace_back(std::make_unique<Token::String>(
                str, file.position().line(), token_start, file.name()));
          } else if ('\'' == next) {
            auto &&character{line.substr(begin, end)};
#ifdef ADDONS
            if (character.size() > 1 && '\\' != character.front()) {
              throwError(
                  this,
                  Diagnostics::Diagnostic(
                      std::make_unique<Diagnostics::DiagnosticHighlighter>(
                          token_start, character.length(), file.line()),
                      fmt::format("Invalid character literal '{}'", character),
                      file.name(), file.position().line()));
            } else {
              lTokens.emplace_back(std::make_unique<Token::Character>(
                  character, file.position().line(), file.position().column(),
                  file.name()));
            }
#else
            throwError(this,
                       Diagnostics::Diagnostic(
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
          file.setColumn(line.index() - 1);
          if ('/' != line.next()) {
            throwWarning(
                this, Diagnostics::Diagnostic(
                          std::make_unique<Diagnostics::DiagnosticHighlighter>(
                              file.position().column(), 0, file.line()),
                          "Found '/', acting as if it's supposed to be '//'",
                          file.name(), file.position().line()));
          }
          [[fallthrough]];
        case ';':
#ifdef KEEP_COMMENTS
          lTokens.emplace_back(std::make_unique<Token::Comment>(
              line.substr(line.index(), -1), file.position().line(),
              file.position().column(), file.name()));
#endif
          line.skip_while([](auto &&) -> bool { return true; });
          break;
        case '#': {
          token = line.substr(token_start, line.find_if([](auto &&c) {
            return !(std::isdigit(c) || '-' == c);
          }));
          if (isValidDecimalLiteral(token)) {
            auto &&decimal{std::make_unique<Token::Decimal>(
                token, file.position().line(), file.position().column(),
                file.name())};
            if (decimal->isTooBig()) {
              throwError(
                  this,
                  Diagnostics::Diagnostic(
                      std::make_unique<Diagnostics::DiagnosticHighlighter>(
                          token_start, decimal->getToken().length(),
                          file.line()),
                      fmt::format("Immediate literal requires more than 16 "
                                  "bits to represent"),
                      file.name(), file.position().line()));
              lTokens.emplace_back(std::make_unique<Token::Token>(
                  token, file.position().line(), file.position().column(),
                  file.name()));
            } else {
              lTokens.emplace_back(std::move(decimal));
            }
          } else {
            lTokens.emplace_back(std::make_unique<Token::Token>(
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
        if (auto &&tToken{tokenize(token)};
            tToken->tokenType() == TokenType::IMMEDIATE && tToken->isTooBig()) {
          throwError(
              this,
              Diagnostics::Diagnostic(
                  std::make_unique<Diagnostics::DiagnosticHighlighter>(
                      token_start, tToken->getToken().length(), file.line()),
                  fmt::format("Immediate literal requires more than 16 "
                              "bits to represent"),
                  file.name(), file.position().line()));
          lTokens.emplace_back(std::make_unique<Token::Token>(
              token, file.position().line(), file.position().column(),
              file.name()));
        } else {
          lTokens.emplace_back(std::move(tToken));
        }
        terminator = '\0';
      }
    }

    return lTokens;
  }

  Lexer &lexer() const { return m_lexer; }

private:
  Lexer &m_lexer;
  File &file;
}; // namespace Tokenizer
} // namespace Tokenizer
} // namespace Lexer

#endif
