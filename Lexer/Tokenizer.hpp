#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "../Debug.hpp"

#include "Line.hpp"
#include "Token.hpp"
#include "Tokens.hpp"

static constexpr size_t hashed_letters[] = {
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
    return 0;
  }

  const size_t first_character = static_cast<size_t>(toUpper(string.front()));

  if (first_character != '.' && !isAlpha(first_character)) {
    // Basically, we don't really want something that's likely to be an
    // immediate value, or a label (less likely to be caught here, but may as
    // well try).
    return 0;
  } else {
    size_t hashed{37};

    for (const char character : string) {
      const size_t as_hashed{
          hashed_letters[(static_cast<size_t>(toUpper(character)) - 0x41u) &
                         0x1F]};
      hashed = (hashed * as_hashed) ^ (first_character * as_hashed);
    }

    return hashed;
  }
}

bool is_valid_binary_literal(const std::string_view &s) {
  if (s.size() == 1) {
    return false;
  }

  if ('B' == toUpper(s.front())) {
    return std::all_of(s.cbegin() + 1, s.cend(),
                       [](auto &&c) { return '0' == c || '1' == c; });
  }

  if (s.size() > 2) {
    return std::all_of(s.cbegin() + 2, s.cend(),
                       [](auto &&c) { return '0' == c || '1' == c; });
  }

  return false;
}

bool is_valid_hexadecimal_literal(const std::string_view &s) {
  if (s.size() == 1) {
    return false;
  }

  if ('X' == toUpper(s.front())) {
    return std::all_of(s.cbegin() + 1, s.cend(), ::isxdigit);
  }

  if (s.size() > 2 && 'X' == toUpper(s[1])) {
    return std::all_of(s.cbegin() + 2, s.cend(), ::isxdigit);
  }

  return false;
}

bool is_valid_octal_literal(const std::string_view &s) {
  return std::all_of(s.cbegin(), s.cend(),
                     [](auto &&c) { return c <= 0x37 && c >= 0x30; });
}

bool is_valid_decimal_literal(const std::string_view &s) {
  return std::all_of(s.cbegin(), s.cend(), ::isdigit);
}

bool is_valid_label(const std::string_view &s) {
  if ('.' == s.front()) {
    return std::all_of(s.cbegin() + 1, s.cend(),
                       [](auto &&c) { return std::isalnum(c) || '_' == c; });
  }

  return std::all_of(s.cbegin(), s.cend(),
                     [](auto &&c) { return std::isalnum(c) || '_' == c; });
}

namespace Lexer {
namespace Tokenizer {
Token::Token *tokenize_immediate(const std::string &s) {
  switch (s.front()) {
  case '\'':
    break;
  case '0':
    if (s.size() > 1) {
      if ('X' == toUpper(s[1]) && is_valid_hexadecimal_literal(s)) {
        return new Token::Hexadecimal(s);
      } else if ('B' == toUpper(s[1]) && is_valid_binary_literal(s)) {
        return new Token::Binary(s);
      }
      if (is_valid_octal_literal(s)) {
        return new Token::Octal(s);
      }
      Debug::console->debug("Invalid immediate literal {}", s);
    }
    break;
  case 'b':
    [[fallthrough]];
  case 'B':
    if (is_valid_binary_literal(s)) {
      return new Token::Binary(s);
    } else {
      Debug::console->debug("Expected Binary literal, but found {}", s);
    }
    break;
  case 'x':
    [[fallthrough]];
  case 'X':
    if (is_valid_hexadecimal_literal(s)) {
      return new Token::Hexadecimal(s);
    } else {
      Debug::console->debug("Expected Hexadecimal literal, but found {}", s);
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
    if (is_valid_decimal_literal(s)) {
      return new Token::Decimal(s);
    } else {
      Debug::console->debug("Expected Decimal literal, but found {}", s);
    }
    break;
  default:
    break;
  }

  return new Token::Token(s);
}

Token::Token *tokenize_directive(const std::string &s) {
  if (const auto _hash = hash(s); _hash > 0) {
    switch (_hash) {
#ifdef ADDONS
    case ::hash("INCLUDE"):
      return new Token::Include(s);
    case ::hash("SET"):
      return new Token::Set(s);
    case ::hash("LSHIFT"):
      return new Token::Lshift(s);
    case ::hash("NEG"):
      return new Token::Neg(s);
    case ::hash("SUB"):
      return new Token::Sub(s);
#endif
    case ::hash("ORIG"):
      return new Token::Orig(s);
    case ::hash("END"):
      return new Token::End(s);
    case ::hash("STRINGZ"):
      return new Token::Stringz(s);
    case ::hash("FILL"):
      return new Token::Fill(s);
    case ::hash("BLKW"):
      return new Token::Blkw(s);
    }
  }

  if (is_valid_label(s)) {
    return new Token::Label(s);
  }

  return new Token::Token(s);
}

/*! Tokenize a single word
 *
 * @param s The word to tokenize
 * @return The token the word corresponds to
 */
Token::Token *tokenize(const std::string &s) {
  if (const auto _hash = hash(s); _hash > 0) {
    switch (_hash) {
    case ::hash("ADD"):
      return new Token::Add(s);
    case ::hash("AND"):
      return new Token::And(s);
    case ::hash("NOT"):
      return new Token::Not(s);
    case ::hash("RET"):
      return new Token::Ret(s);
    case ::hash("JMP"):
      return new Token::Jmp(s);
    case ::hash("JSR"):
      return new Token::Jsr(s);
    case ::hash("JSRR"):
      return new Token::Jsrr(s);
    case ::hash("LD"):
      return new Token::Ld(s);
    case ::hash("LEA"):
      return new Token::Lea(s);
    case ::hash("LDI"):
      return new Token::Ldi(s);
    case ::hash("LDR"):
      return new Token::Ldr(s);
    case ::hash("ST"):
      return new Token::St(s);
    case ::hash("STR"):
      return new Token::Str(s);
    case ::hash("STI"):
      return new Token::Sti(s);
    case ::hash("TRAP"):
      return new Token::Trap(s);
    case ::hash("PUTS"):
      return new Token::Puts(s);
    case ::hash("PUTSP"):
      return new Token::Putsp(s);

    case ::hash("PUTC"):
      [[fallthrough]];
    case ::hash("OUT"):
      return new Token::Out(s);

    case ::hash("IN"):
      return new Token::In(s);
    case ::hash("HALT"):
      return new Token::Halt(s);
    case ::hash("RTI"):
      return new Token::Rti(s);

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
      return new Token::Br(s, true, true, true);

    case ::hash("BRN"):
      return new Token::Br(s, true, false, false);
    case ::hash("BRZ"):
      return new Token::Br(s, false, true, false);
    case ::hash("BRP"):
      return new Token::Br(s, false, false, true);

    case ::hash("BRNZ"):
      [[fallthrough]];
    case ::hash("BRZN"):
      return new Token::Br(s, true, true, false);

    case ::hash("BRNP"):
      [[fallthrough]];
    case ::hash("BRPN"):
      return new Token::Br(s, true, false, true);

    case ::hash("BRZP"):
      [[fallthrough]];
    case ::hash("BRPZ"):
      return new Token::Br(s, false, true, true);

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
      return new Token::Register(s);
    default:
      break;
    }
  }

  if (is_valid_octal_literal(s)) {
    return new Token::Octal(s);
  }

  if (is_valid_decimal_literal(s)) {
    return new Token::Decimal(s);
  }

  if (is_valid_hexadecimal_literal(s)) {
    return new Token::Hexadecimal(s);
  }

  if (is_valid_binary_literal(s)) {
    return new Token::Binary(s);
  }

  if (is_valid_label(s)) {
    return new Token::Label(s);
  }

  return new Token::Token(s);
}

static inline void extraneous(const char character) {
  Debug::console->debug("Extraneous '{}'", character);
  Notification::diagnostic_notifications << Diagnostics::Diagnostic(
      "Extraneous " + std::string(1, character) + " found.");
}

std::vector<Token::Token *> tokenize_line(Line line) {
  std::vector<Token::Token *> l_tokens{};
  char terminator{};

  while (!line.at_end()) {
    line.skip_while([](auto &&c) -> bool { return std::isspace(c); });
    const auto token_start = line.index();
    const auto token_end = line.find_if(
        [](auto &&c) -> bool { return !(std::isalnum(c) || '_' == c); });

    Debug::console->debug("Starting with index at {}", token_start);
    Debug::console->debug("Token ends with index at {}", token_end);

    if (auto &&token = line.substr(token_start, token_end); 0 == token.size()) {
      switch (const auto next = line.next(); next) {
      case '.': {
        terminator = '\0';
        // Now we tokenize a directive
        const auto dir_start = line.index();
        token = line.substr(dir_start, line.find_if([](auto &&c) -> bool {
          return !(std::isalnum(c));
        }));
        Debug::console->debug("Found token '{}'", token);
        l_tokens.emplace_back(tokenize_directive(token));
        Debug::console->debug(" - With type ", TokenTypeString[l_tokens.back()->tokenType()]);
        break;
      }
      case '-': { // Negative immediate (hopefully)
        const auto imm_start = line.index();
        token = line.substr(imm_start, line.find_if([](auto &&c) -> bool {
          return !(std::isalnum(c));
        }));
        if (token.size() > 0) {
          auto &&t = tokenize_immediate(token);
          t->negate();
          l_tokens.emplace_back(std::move(t));
        } else {
          Debug::console->debug("Random extra '-'", "");
        }
        break;
      }
      case ',': // Just a seperator between operands, not required.
      case ':': // As above.
        if (',' == terminator || ':' == terminator) {
          extraneous(next);
        } else if (l_tokens.size() > 1) {
          if (':' == next)
            if (l_tokens.back()->tokenType() != Token_Type::LABEL) {
              extraneous(next);
            }
        } else if (',' == next) {
          if (l_tokens.back()->tokenType() != Token_Type::REGISTER ||
              l_tokens.back()->tokenType() != Token_Type::IMMEDIATE) {
            extraneous(next);
          }
        }
        terminator = next;
        break;
      case '"':    // string
      case '\'': { // character
        terminator = '\0';
        line.ignore(Line::ESCAPE_SEQUENCE);
        const auto begin = line.index();
        const auto end = line.find_next(next);
        line.ignore(Line::RESET);
        if (-1u == end) {
          Debug::console->debug("Unterminated string/character literal", "");
        } else if ('"' == next) {
          auto &&str = line.substr(begin, end);
          Debug::console->debug("Found string \"{}\"", str);
          l_tokens.emplace_back(new Token::String(str));
        } else if ('\'' == next) {
#ifdef ADDONS
          if (auto &&character = line.substr(begin, end);
              character.size() > 1 && '\\' != character.front()) {
            Debug::console->debug("Invalid character literal '{}'", character);
          } else {
            Debug::console->debug("Found character '{}'", character);
            l_tokens.emplace_back(new Token::Character(character));
          }
#else
          debug("Found character literal, but Addons aren't enabled");
#endif
        }
        break;
      }
      case ';':
        [[fallthrough]];
      case '/': // Both mean a comment (though, technically, a '//' is a
        // comment, but '/' is not used for anything.)
        line.skip_while([](auto &&) -> bool { return true; });
        break;
      default:
        terminator = '\0';
        break;
      }
    } else {
      Debug::console->debug("Found token '{}'", token);
      l_tokens.emplace_back(tokenize(token));
      Debug::console->debug(" - With type {}", TokenTypeString[l_tokens.back()->tokenType()]);
      terminator = '\0';
    }
  }

  return l_tokens;
} // namespace Tokenizer
} // namespace Tokenizer
} // namespace Lexer

#endif
