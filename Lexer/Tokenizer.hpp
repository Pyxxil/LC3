#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "Debug.hpp"

#include "Lexer/Tokens/Add.hpp"
#include "Lexer/Tokens/And.hpp"
#include "Lexer/Tokens/Binary.hpp"
#include "Lexer/Tokens/Blkw.hpp"
#include "Lexer/Tokens/Br.hpp"
#include "Lexer/Tokens/Character.hpp"
#include "Lexer/Tokens/Decimal.hpp"
#include "Lexer/Tokens/End.hpp"
#include "Lexer/Tokens/Fill.hpp"
#include "Lexer/Tokens/Halt.hpp"
#include "Lexer/Tokens/Hexadecimal.hpp"
#include "Lexer/Tokens/In.hpp"
#include "Lexer/Tokens/Include.hpp"
#include "Lexer/Tokens/Jmp.hpp"
#include "Lexer/Tokens/Jsr.hpp"
#include "Lexer/Tokens/Jsrr.hpp"
#include "Lexer/Tokens/Label.hpp"
#include "Lexer/Tokens/Ld.hpp"
#include "Lexer/Tokens/Ldi.hpp"
#include "Lexer/Tokens/Ldr.hpp"
#include "Lexer/Tokens/Lea.hpp"
#include "Lexer/Tokens/Lshift.hpp"
#include "Lexer/Tokens/Neg.hpp"
#include "Lexer/Tokens/Not.hpp"
#include "Lexer/Tokens/Octal.hpp"
#include "Lexer/Tokens/Orig.hpp"
#include "Lexer/Tokens/Out.hpp"
#include "Lexer/Tokens/Puts.hpp"
#include "Lexer/Tokens/Putsp.hpp"
#include "Lexer/Tokens/Register.hpp"
#include "Lexer/Tokens/Ret.hpp"
#include "Lexer/Tokens/Rti.hpp"
#include "Lexer/Tokens/Set.hpp"
#include "Lexer/Tokens/St.hpp"
#include "Lexer/Tokens/Sti.hpp"
#include "Lexer/Tokens/Str.hpp"
#include "Lexer/Tokens/String.hpp"
#include "Lexer/Tokens/Stringz.hpp"
#include "Lexer/Tokens/Sub.hpp"
#include "Lexer/Tokens/Trap.hpp"
#include "Line.hpp"
#include "Token.hpp"

static constexpr size_t hashed_letters[] = {
  100363, 99989, 97711, 97151, 92311, 80147, 82279, 72997, 66457, 65719, 70957,
  50262,  48407, 51151, 41047, 39371, 35401, 37039, 28697, 27791, 20201, 21523,
  6449,   4813,  16333, 13337, 3571,  5519,  26783, 71471, 68371, 104729
};

static constexpr char
toUpper(char c)
{
  return (c >= 0x61 && c <= 0x7a) ? static_cast<char>(c - 0x20) : c;
}

/*! Create a hash of a c-string.
 *
 * @param string The c-string to hash.
 * @param length The length of the c-string.
 *
 * @return The hash.
 */
static constexpr size_t
hash(const char* const string, size_t length)
{
  size_t hashed{ 37 };
  const size_t first_character = static_cast<size_t>(toUpper(*string));

  for (size_t index = 0; index < length; ++index) {
    const size_t as_hashed{
      hashed_letters[(static_cast<size_t>(toUpper(string[index])) - 0x41u) &
                     0x1F]
    };
    hashed = (hashed * as_hashed) ^ (first_character * as_hashed);
  }

  return hashed;
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
static size_t
hash(const std::string& string)
{
  const size_t first_character =
    static_cast<size_t>(std::toupper(string.front()));
  if (first_character != '.' &&
      (0 == std::isprint(static_cast<int>(first_character)))) {
    // Basically, we don't really want something that's likely to be an
    // immediate value, or a label (less likely to be caught here, but may as
    // well try).
    return 0;
  }

  if (string.length() > 8 || string.length() < 2) {
    // There are no registers, directives, or instructions which are longer than
    // 8 characters, or less than 2, so this string isn't one of them.
    return 0;
  }

  size_t hashed{ 37 };

  for (const auto character : string) {
    const size_t as_hashed{
      hashed_letters[(static_cast<size_t>(std::toupper(character)) - 0x41u) &
                     0x1F]
    };
    hashed = (hashed * as_hashed) ^ (first_character * as_hashed);
  }

  return hashed;
}

bool
is_valid_binary_literal(const std::string& s)
{
  if (s.size() == 1) {
    return false;
  }

  if ('B' == toUpper(s.front())) {
    return std::all_of(
      s.cbegin() + 1, s.cend(), [](auto&& c) { return '0' == c || '1' == c; });
  }

  if (s.size() > 2) {
    return std::all_of(
      s.cbegin() + 2, s.cend(), [](auto&& c) { return '0' == c || '1' == c; });
  }

  return false;
}

bool
is_valid_hexadecimal_literal(const std::string& s)
{
  if (s.size() == 1) {
    return false;
  }

  if ('X' == toUpper(s.front())) {
    debug("HEXADECIMAL LITERAL STARTING WITH X");
    return std::all_of(s.cbegin() + 1, s.cend(), ::isxdigit);
  }

  if (s.size() > 2 && 'X' == toUpper(s[1])) {
    return std::all_of(s.cbegin() + 2, s.cend(), ::isxdigit);
  }

  return false;
}

bool
is_valid_octal_literal(const std::string& s)
{
  return std::all_of(
    s.cbegin(), s.cend(), [](auto&& c) { return c <= 0x37 && c >= 0x30; });
}

bool
is_valid_decimal_literal(const std::string& s)
{
  return std::all_of(s.cbegin(), s.cend(), ::isdigit);
}

bool
is_valid_label(const std::string& s)
{
  if ('.' == s.front()) {
    return std::all_of(s.cbegin() + 1, s.cend(), [](auto&& c) {
      return std::isalnum(c) || '_' == c;
    });
  }

  return std::all_of(
    s.cbegin(), s.cend(), [](auto&& c) { return std::isalnum(c) || '_' == c; });
}

namespace Lexer {
class Tokenizer
{
public:
  Tokenizer() = default;

  Tokenizer(const Tokenizer&) = default;
  Tokenizer(Tokenizer&&) noexcept = default;

  Tokenizer& operator=(const Tokenizer&) = default;
  Tokenizer& operator=(Tokenizer&&) noexcept = default;

  ~Tokenizer() = default;

  std::shared_ptr<Token::Token> tokenize_immediate(const std::string& s)
  {
    switch (s.front()) {
      case '\'':
        break;
      case '0':
        if (s.size() > 1) {
          if ('X' == toUpper(s[1]) && is_valid_hexadecimal_literal(s)) {
            return std::make_shared<Token::Hexadecimal>(s);
          } else if ('B' == toUpper(s[1]) && is_valid_binary_literal(s)) {
            return std::make_shared<Token::Binary>(s);
          }
          if (is_valid_octal_literal(s)) {
            return std::make_shared<Token::Octal>(s);
          }
          debug("Invalid immediate literal " << s);
        }
        break;
      case 'b':
      case 'B':
        if (is_valid_binary_literal(s)) {
          return std::make_shared<Token::Binary>(s);
        } else {
          debug("Expected Binary literal, but found " << s);
        }
        break;
      case 'x':
      case 'X':
        if (is_valid_hexadecimal_literal(s)) {
          return std::make_shared<Token::Hexadecimal>(s);
        } else {
          debug("Expected Hexadecimal literal, but found " << s);
        }
        break;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (is_valid_decimal_literal(s)) {
          return std::make_shared<Token::Decimal>(s);
        } else {
          debug("Expected Decimal literal, but found " << s);
        }
        break;
    }

    return std::make_shared<Token::Token>(s);
  }

  std::shared_ptr<Token::Token> tokenize_directive(const std::string& s)
  {
    const auto _hash = hash(s);
    if (_hash > 0) {
      switch (_hash) {
        case ::hash("INCLUDE", 7):
          return std::make_shared<Token::Include>(s);
        case ::hash("STRINGZ", 7):
          return std::make_shared<Token::Stringz>(s);
        case ::hash("FILL", 4):
          return std::make_shared<Token::Fill>(s);
        case ::hash("BLKW", 4):
          return std::make_shared<Token::Blkw>(s);
        case ::hash("SET", 3):
          return std::make_shared<Token::Set>(s);
        case ::hash("LSHIFT", 6):
          return std::make_shared<Token::Lshift>(s);
        case ::hash("NEG", 3):
          return std::make_shared<Token::Neg>(s);
        case ::hash("SUB", 3):
          return std::make_shared<Token::Sub>(s);
        case ::hash("ORIG", 4):
          return std::make_shared<Token::Orig>(s);
        case ::hash("END", 3):
          return std::make_shared<Token::End>(s);
      }
    }
    // Check if it's a valid label
    return std::make_shared<Token::Token>(s);
  }

  /*! Tokenize a single word
   *
   * @param s The word to tokenize
   * @return The token the word corresponds to
   */
  std::shared_ptr<Token::Token> tokenize(const std::string& s)
  {
    const auto _hash = hash(s);
    if (_hash > 0) {
      switch (_hash) {
        case ::hash("ADD", 3):
          return std::make_shared<Token::Add>(s);
        case ::hash("AND", 3):
          return std::make_shared<Token::And>(s);
        case ::hash("NOT", 3):
          return std::make_shared<Token::Not>(s);
        case ::hash("RET", 3):
          return std::make_shared<Token::Ret>(s);
        case ::hash("JMP", 3):
          return std::make_shared<Token::Jmp>(s);
        case ::hash("JSR", 3):
          return std::make_shared<Token::Jsr>(s);
        case ::hash("JSRR", 4):
          return std::make_shared<Token::Jsrr>(s);
        case ::hash("LD", 2):
          return std::make_shared<Token::Ld>(s);
        case ::hash("LEA", 3):
          return std::make_shared<Token::Lea>(s);
        case ::hash("LDI", 3):
          return std::make_shared<Token::Ldi>(s);
        case ::hash("LDR", 3):
          return std::make_shared<Token::Ldr>(s);
        case ::hash("ST", 2):
          return std::make_shared<Token::St>(s);
        case ::hash("STR", 3):
          return std::make_shared<Token::Str>(s);
        case ::hash("STI", 3):
          return std::make_shared<Token::Sti>(s);
        case ::hash("TRAP", 4):
          return std::make_shared<Token::Trap>(s);
        case ::hash("PUTS", 4):
          return std::make_shared<Token::Puts>(s);
        case ::hash("PUTSP", 5):
          return std::make_shared<Token::Putsp>(s);

        case ::hash("PUTC", 4): // FALLTHROUGH
        case ::hash("OUT", 3):
          return std::make_shared<Token::Out>(s);

        case ::hash("IN", 2):
          return std::make_shared<Token::In>(s);
        case ::hash("HALT", 4):
          return std::make_shared<Token::Halt>(s);
        case ::hash("RTI", 3):
          return std::make_shared<Token::Rti>(s);

        case ::hash("BR", 2):    // FALLTHROUGH
        case ::hash("BRNZP", 5): // FALLTHROUGH
        case ::hash("BRNPZ", 5): // FALLTHROUGH
        case ::hash("BRZNP", 5): // FALLTHROUGH
        case ::hash("BRZPN", 5): // FALLTHROUGH
        case ::hash("BRPNZ", 5): // FALLTHROUGH
        case ::hash("BRPZN", 5):
          return std::make_shared<Token::Br>(s, true, true, true);

        case ::hash("BRN", 3):
          return std::make_shared<Token::Br>(s, true, false, false);
        case ::hash("BRZ", 3):
          return std::make_shared<Token::Br>(s, false, true, false);
        case ::hash("BRP", 3):
          return std::make_shared<Token::Br>(s, false, false, true);

        case ::hash("BRNZ", 4): // FALLTHROUGH
        case ::hash("BRZN", 4):
          return std::make_shared<Token::Br>(s, true, true, false);

        case ::hash("BRNP", 4): // FALLTHROUGH
        case ::hash("BRPN", 4):
          return std::make_shared<Token::Br>(s, true, false, true);

        case ::hash("BRZP", 4): // FALLTHROUGH
        case ::hash("BRPZ", 4):
          return std::make_shared<Token::Br>(s, false, true, true);

        case ::hash("R0", 2): // FALLTHROUGH
        case ::hash("R1", 2): // FALLTHROUGH
        case ::hash("R2", 2): // FALLTHROUGH
        case ::hash("R3", 2): // FALLTHROUGH
        case ::hash("R4", 2): // FALLTHROUGH
        case ::hash("R5", 2): // FALLTHROUGH
        case ::hash("R6", 2): // FALLTHROUGH
        case ::hash("R7", 2):
          return std::make_shared<Token::Register>(s);
        default:
          break;
      }
    }

    if (is_valid_binary_literal(s)) {
      if (s == "END") {
        debug("END IS A BINARY LITERAL");
      }
      return std::make_shared<Token::Binary>(s);
    }

    if (is_valid_decimal_literal(s)) {
      if (s == "END") {
        debug("END IS A DECIMAL LITERAL");
      }
      return std::make_shared<Token::Decimal>(s);
    }

    if (is_valid_hexadecimal_literal(s)) {
      if (s == "END") {
        debug("END IS A HEXADECIMAL LITERAL");
      }
      return std::make_shared<Token::Hexadecimal>(s);
    }

    if (is_valid_octal_literal(s)) {
      if (s == "END") {
        debug("END IS A OCTAL LITERAL");
      }
      return std::make_shared<Token::Octal>(s);
    }

    if (is_valid_label(s)) {
      return std::make_shared<Token::Label>(s);
    }

    return std::make_shared<Token::Token>(s);
  }

  std::vector<std::shared_ptr<Token::Token>> tokenize_line(Line line)
  {
    std::vector<std::shared_ptr<Token::Token>> l_tokens;
    char terminator{};

    while (!line.at_end()) {
      line.skip_while([](auto&& c) -> bool { return std::isspace(c); });
      const auto start = line.index();
      auto&& token = line.substr(start, line.find_if([](auto&& c) -> bool {
        return !(std::isalnum(c) || '_' == c);
      }));
      if (0 == token.size()) {
        const auto next = line.next();
        switch (next) {
          case '.': {
            terminator = '\0';
            // Now we tokenize a directive
            const auto dir_start = line.index();
            token = line.substr(dir_start, line.find_if([](auto&& c) -> bool {
              return !(std::isalnum(c));
            }));
            l_tokens.emplace_back(tokenize_directive(token));
            break;
          }
          case '-': {
            const auto imm_start = line.index();
            token = line.substr(imm_start, line.find_if([](auto&& c) -> bool {
              return !(std::isalnum(c));
            }));
            if (token.size() > 0) {
              auto&& t = tokenize_immediate(token);
              t->negate();
              l_tokens.emplace_back(std::move(t));
            } else {
              debug("Random extra '-'");
            }
            break;
          }
          case ',':
          case ':':
            if (',' == terminator || ':' == terminator ||
                (':' == terminator &&
                 l_tokens.back()->tokenType() != Token_Type::LABEL)) {
              debug("Extraneous '" << next << '\'');
            }
            terminator = next;
            break;
          case '"':
          case '\'': {
            terminator = '\0';
            line.ignore(Line::ESCAPE_SEQUENCE);
            const auto begin = line.index();
            const auto end = line.find_next(next) - 1;
            line.ignore(Line::RESET);
            if (-1u == end) {
              debug("Unterminated string/character literal");
            } else if ('"' == next) {
              auto&& str = line.substr(begin, end);
              debug("Found string \"" << str << '"');
              l_tokens.emplace_back(std::make_shared<Token::String>(str));
            } else if ('\'' == next) {
#ifdef ADDONS
              auto&& character = line.substr(begin, end);
              if (character.size() > 1 && '\\' != character.front()) {
                debug("Invalid character literal " << character);
              } else {
                debug("Found character '" << character << '\'');
                l_tokens.emplace_back(
                  std::make_shared<Token::Character>(character));
              }
#else
              debug("Found character literal, but Addons aren't enabled");
#endif
            }
            break;
          }
          case ';': // FALLTHROUGH
          case '/':
            // Comment
            line.skip_while([](auto &&) -> bool { return true; });
            break;
          default:
            terminator = '\0';
            break;
        }
      } else {
        debug("Found token '" << token << '\'');
        l_tokens.emplace_back(tokenize(token));
        debug(" - With type " << TokenTypeString[l_tokens.back()->tokenType()]);
        terminator = '\0';
      }
    }

    return l_tokens;
  }

private:
};
}

#endif
