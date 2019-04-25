#include "Tokenizer.hpp"

namespace {

constexpr int UPPER_TOGGLE = 0x20;
constexpr int LOWER_CASE_A = 0x61;
constexpr int UPPER_CASE_A = LOWER_CASE_A ^ UPPER_TOGGLE;
constexpr int UPPER_CASE_F = 0x46;
constexpr int LOWER_CASE_Z = 0x7A;
constexpr int UPPER_CASE_Z = LOWER_CASE_Z ^ UPPER_TOGGLE;
constexpr int ZERO = 0x30;
constexpr int ONE = 0x31;
constexpr int SEVEN = 0x37;
constexpr int NINE = 0x39;
constexpr size_t LAST_FIVE_BIT_MASK = 0x1F;

constexpr size_t STARTING_HASH = 37;

constexpr size_t HASHED_LETTERS[] = {
    100363, 99989, 97711, 97151, 92311, 80147, 82279, 72997,
    66457,  65719, 70957, 50262, 48407, 51151, 41047, 39371,
    35401,  37039, 28697, 27791, 20201, 21523, 6449,  4813,
    16333,  13337, 3571,  5519,  26783, 71471, 68371, 104729};

constexpr int to_upper(int c) {
  return (c >= LOWER_CASE_A && c <= LOWER_CASE_Z) ? (c ^ UPPER_TOGGLE) : c;
}

constexpr bool is_alpha(int c) {
  return (c >= UPPER_CASE_A && c <= UPPER_CASE_Z) ||
         (c >= LOWER_CASE_A && c <= LOWER_CASE_Z);
}

constexpr bool is_digit(char c) { return c >= ZERO && c <= NINE; }

constexpr bool is_bin_digit(char c) { return c == ZERO || c == ONE; }

constexpr bool is_oct_digit(char c) { return c >= ZERO && c <= SEVEN; }

constexpr bool is_hex_digit(char c) {
  const auto upper = to_upper(c);
  return is_digit(c) || (upper >= UPPER_CASE_A && upper <= UPPER_CASE_F);
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

  size_t hashed{STARTING_HASH};

  for (auto character : string) {
    const size_t as_hashed{
        HASHED_LETTERS[(static_cast<size_t>(to_upper(character)) -
                        UPPER_CASE_A) &
                       LAST_FIVE_BIT_MASK]};
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
  }

  if (s.length() == 2) {
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

struct TokenValues {
  TokenValues(std::string_view s, size_t line, size_t column,
              const std::string &file_name)
      : s(std::string(s)), line(line), column(column), file_name(file_name) {}
  std::string s;
  size_t line;
  size_t column;
  const std::string &file_name;

  template <typename T> constexpr auto to() const {
    return std::make_unique<T>(s, line, column, file_name);
  }

  template <typename T, typename E> constexpr auto to(E extra) const {
    // For immeduate values
    return std::make_unique<T>(s, line, column, file_name, extra);
  }

  template <typename T, typename E> constexpr auto to(E e1, E e2, E e3) const {
    // For branch instructions
    return std::make_unique<T>(s, e1, e2, e3, line, column, file_name);
  }
};

} // namespace

namespace Lexer::Tokenizer {

Tokenizer::Tokenizer::Tokenizer(Lexer &t_lexer, File &t_file)
    : m_lexer(t_lexer), file(t_file) {}

std::unique_ptr<Token::Token>
Tokenizer::tokenize_immediate(std::string_view s) {
  // We know this is going to be negative, as this is only ever called when
  // the next character on the line is a '-'.
  const TokenValues values{s, file.position().line(), file.position().column(),
                           file.name()};
  switch (to_upper(s.front())) {
  case '\'':
    break;
  case '0':
    if (s.length() > 1) {
      if ('X' == to_upper(s[1]) && is_valid_hexadecimal_literal(s)) {
        auto hex{values.to<Token::Hexadecimal>()};
        if (!hex->is_too_big()) {
          return std::move(hex);
        }
        throw_error(this,
                    Diagnostics::Diagnostic(
                        std::make_unique<Diagnostics::DiagnosticHighlighter>(
                            file.position().column() - s.length(), s.length(),
                            file.line()),
                        "Hexadecimal literal is too big to fit inside 16 bits",
                        file.name(), file.position().line()));
        return values.to<Token::Token>();
      } else if ('B' == to_upper(s[1]) && is_valid_binary_literal(s)) {
        auto bin{values.to<Token::Binary>(true)};
        if (!bin->is_too_big()) {
          return std::move(bin);
        }
        throw_error(this,
                    Diagnostics::Diagnostic(
                        std::make_unique<Diagnostics::DiagnosticHighlighter>(
                            file.position().column() - s.length(), s.length(),
                            file.line()),
                        "Binary literal is too big to fit inside 16 bits",
                        file.name(), file.position().line()));
        return values.to<Token::Token>();
      }

#ifdef ADDONS
      if (is_valid_octal_literal(s)) {
        auto oct{values.to<Token::Octal>(true)};
        if (!oct->is_too_big()) {
          return std::move(oct);
        }
        throw_error(this,
                    Diagnostics::Diagnostic(
                        std::make_unique<Diagnostics::DiagnosticHighlighter>(
                            file.position().column() - s.length(), s.length(),
                            file.line()),
                        "Octal literal is too big to fit inside 16 bits",
                        file.name(), file.position().line()));
        return values.to<Token::Token>();
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
      auto bin{values.to<Token::Binary>(true)};
      if (!bin->is_too_big()) {
        return std::move(bin);
      }
      throw_error(this,
                  Diagnostics::Diagnostic(
                      std::make_unique<Diagnostics::DiagnosticHighlighter>(
                          file.position().column() - s.length(), s.length(),
                          file.line()),
                      "Binary literal is too big to fit inside 16 bits",
                      file.name(), file.position().line()));
      return values.to<Token::Token>();
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
      auto hex{values.to<Token::Hexadecimal>(true)};
      if (!hex->is_too_big()) {
        return std::move(hex);
      }
      throw_error(this,
                  Diagnostics::Diagnostic(
                      std::make_unique<Diagnostics::DiagnosticHighlighter>(
                          file.position().column() - s.length(), s.length(),
                          file.line()),
                      "Hexadecimal literal is too big to fit inside 16 bits",
                      file.name(), file.position().line()));
      return values.to<Token::Token>();
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
      auto decimal{values.to<Token::Decimal>(true)};
      if (!decimal->is_too_big()) {
        return std::move(decimal);
      }
      throw_error(this,
                  Diagnostics::Diagnostic(
                      std::make_unique<Diagnostics::DiagnosticHighlighter>(
                          file.position().column() - s.length(), s.length(),
                          file.line()),
                      "Decimal literal is too big to fit inside 16 bits",
                      file.name(), file.position().line()));
      return values.to<Token::Token>();
    } else {
      throw_error(this,
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

  return values.to<Token::Token>();
}

std::unique_ptr<Token::Token>
Tokenizer::tokenize_directive(std::string_view s) {
  const auto &[column, line] = file.position();
  const TokenValues vals{s, line, column, file.name()};
  if (const auto _hash{hash(s)}; _hash > 0) {
    switch (_hash) {
    case ::hash(".FILL"):
      return vals.to<Token::Fill>();
    case ::hash(".STRINGZ"):
      return vals.to<Token::Stringz>();
    case ::hash(".BLKW"):
      return vals.to<Token::Blkw>();
#ifdef ADDONS
    case ::hash(".INCLUDE"):
      return vals.to<Token::Include>();
    case ::hash(".SET"):
      return vals.to<Token::Set>();
    case ::hash(".LSHIFT"):
      return vals.to<Token::Lshift>();
    case ::hash(".NEG"):
      return vals.to<Token::Neg>();
    case ::hash(".SUB"):
      return vals.to<Token::Sub>();
#endif
    case ::hash(".ORIG"):
      return vals.to<Token::Orig>();
    case ::hash(".END"):
      return vals.to<Token::End>();
    }
  }

  return is_valid_label(s) ? vals.to<Token::Label>() : vals.to<Token::Token>();
}

std::unique_ptr<Token::Token> Tokenizer::tokenize(std::string_view s) {
  const auto &[column, line] = file.position();
  const TokenValues vals{s, line, column, file.name()};
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
      return vals.to<Token::Register>();

    case ::hash("ADD"):
      return vals.to<Token::Add>();
    case ::hash("AND"):
      return vals.to<Token::And>();
    case ::hash("LD"):
      return vals.to<Token::Ld>();
    case ::hash("NOT"):
      return vals.to<Token::Not>();
    case ::hash("RET"):
      return vals.to<Token::Ret>();
#ifdef ADDONS
    case ::hash("JMPT"):
      return vals.to<Token::Jmpt>();
#endif
    case ::hash("JMP"):
      return vals.to<Token::Jmp>();
    case ::hash("JSR"):
      return vals.to<Token::Jsr>();
    case ::hash("JSRR"):
      return vals.to<Token::Jsrr>();
    case ::hash("LEA"):
      return vals.to<Token::Lea>();
    case ::hash("LDI"):
      return vals.to<Token::Ldi>();
    case ::hash("LDR"):
      return vals.to<Token::Ldr>();
    case ::hash("ST"):
      return vals.to<Token::St>();
    case ::hash("STR"):
      return vals.to<Token::Str>();
    case ::hash("STI"):
      return vals.to<Token::Sti>();
    case ::hash("TRAP"):
      return vals.to<Token::Trap>();
    case ::hash("PUTS"):
      return vals.to<Token::Puts>();
    case ::hash("PUTSP"):
      return vals.to<Token::Putsp>();

    case ::hash("PUTC"):
      [[fallthrough]];
    case ::hash("OUT"):
      return vals.to<Token::Out>();

    case ::hash("GETC"):
      return vals.to<Token::Getc>();

    case ::hash("IN"):
      return vals.to<Token::In>();
    case ::hash("HALT"):
      return vals.to<Token::Halt>();
    case ::hash("RTI"):
      return vals.to<Token::Rti>();

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
      return vals.to<Token::Br>(true, true, true);

    case ::hash("BRN"):
      return vals.to<Token::Br>(true, false, false);
    case ::hash("BRZ"):
      return vals.to<Token::Br>(false, true, false);
    case ::hash("BRP"):
      return vals.to<Token::Br>(false, false, true);

    case ::hash("BRNZ"):
      [[fallthrough]];
    case ::hash("BRZN"):
      return vals.to<Token::Br>(true, true, false);

    case ::hash("BRNP"):
      [[fallthrough]];
    case ::hash("BRPN"):
      return vals.to<Token::Br>(true, false, true);

    case ::hash("BRZP"):
      [[fallthrough]];
    case ::hash("BRPZ"):
      return vals.to<Token::Br>(false, true, true);

    default:
      break;
    }
  }

#ifdef ADDONS
  if ('0' == s.front() && is_valid_octal_literal(s)) {
    return vals.to<Token::Octal>();
  }
#endif

  if (is_valid_decimal(s)) {
    return vals.to<Token::Decimal>();
  }

  // Due to how immediate literals are processed, this needs to occur before
  // the hex check, as otherwise it'll be treated as a valid hex digit (and
  // quite possibly seen as too big)
  if (is_valid_binary_literal(s)) {
    return vals.to<Token::Binary>();
  }

  if (is_valid_hexadecimal_literal(s)) {
    return vals.to<Token::Hexadecimal>();
  }

  if (is_valid_label(s)) {
    return vals.to<Token::Label>();
  }

  throw_error(this,
              Diagnostics::Diagnostic(
                  std::make_unique<Diagnostics::DiagnosticHighlighter>(
                      column, s.length(), file.line()),
                  fmt::format("Invalid token: {}", s), file.name(), line));
  return vals.to<Token::Token>();
}

void Tokenizer::extraneous(char character) {
  throw_warning(this, Diagnostics::Diagnostic(
                          std::make_unique<Diagnostics::DiagnosticHighlighter>(
                              file.position().column(), 0, file.line()),
                          fmt::format("Extraneous '{}' found.", character),
                          file.name(), file.position().line()));
}

std::vector<std::unique_ptr<Token::Token>> Tokenizer::tokenize_line(Line line) {
  std::vector<std::unique_ptr<Token::Token>> l_tokens{};
  char terminator{};

  while (!line.at_end()) {
    line.skip_while([](auto &&c) -> bool { return std::isspace(c); });

    const auto token_start{line.index()};
    const auto token_end{line.find_if([](auto &&c) -> bool {
      return !(std::isalnum(c) || '_' == c || '-' == c);
    })};

    file.set_column(token_start);

    if (token_end == token_start) {
      switch (const auto next{line.next()}; next) {
      case '.': {
        terminator = '\0';
        // Now we tokenize a directive
        // const auto dir_start = line.index();
        auto &&token{line.substr(token_end, line.find_if([](auto &&c) -> bool {
          return !std::isalnum(c);
        }))};
        l_tokens.emplace_back(tokenize_directive(token));
        break;
      }
      case '-': { // Negative immediate (hopefully)
        terminator = '\0';
        auto &&token{line.substr(
            line.index(),
            line.find_if([](auto &&c) -> bool { return !std::isalnum(c); }))};
        if (!token.empty()) {
          l_tokens.emplace_back(tokenize_immediate(token));
        } else {
          throw_error(this,
                      Diagnostics::Diagnostic(
                          std::make_unique<Diagnostics::DiagnosticHighlighter>(
                              file.position().column(), 0, file.line()),
                          "Extraneous '-' found.", file.name(),
                          file.position().line()));
        }
        break;
      }
      case ',': { // Just a seperator between operands, not required.
        if (',' == terminator || ':' == terminator) {
          extraneous(next);
        } else if (l_tokens.empty() ||
                   (l_tokens.back()->token_type() != TokenType::REGISTER &&
                    l_tokens.back()->token_type() != TokenType::IMMEDIATE)) {
          extraneous(next);
        }
        terminator = next;
        break;
      }
      case ':': { // As above.
        if (',' == terminator || ':' == terminator) {
          extraneous(next);
        } else if (l_tokens.empty() ||
                   l_tokens.back()->token_type() != TokenType::LABEL) {
          extraneous(next);
        }
        terminator = next;
        break;
      }
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
          const std::string unterminated_literal{
              next == '"' ? "Unterminated String literal"
                          : "Unterminated Character literal"};
#else
          static const std::string unterminated_literal(
              "Unterminated String literal");
#endif
          throw_error(
              this,
              Diagnostics::Diagnostic(
                  std::make_unique<Diagnostics::DiagnosticHighlighter>(
                      token_start, line.index() - begin + 1, file.line()),
                  unterminated_literal, file.name(), file.position().line()));
          l_tokens.emplace_back(std::make_unique<Token::Token>(
              unterminated_literal, file.position().line(), token_start,
              file.name()));
        } else if ('"' == next) {
          auto &&str{line.substr(begin, end)};
          l_tokens.emplace_back(std::make_unique<Token::String>(
              std::string(str), file.position().line(), token_start,
              file.name()));
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
                std::string(character), file.position().line(),
                file.position().column(), file.name()));
          }
#else
          throw_error(this,
                      Diagnostics::Diagnostic(
                          std::make_unique<Diagnostics::DiagnosticHighlighter>(
                              begin - 1, character.length() + 1, file.line()),
                          "Found character literal, but Addons aren't enabled",
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
            std::string(line.substr(line.index(), -1)), file.position().line(),
            file.position().column(), file.name()));
#endif
        line.skip_while([](auto &&) -> bool { return true; });
        break;
      case '#': {
        std::string token{line.substr(token_start, line.find_if([](auto c) {
          return !(is_digit(c) || '-' == c);
        }))};
        if (is_valid_decimal(token)) {
          auto &&decimal{std::make_unique<Token::Decimal>(
              token, file.position().line(), file.position().column(),
              file.name())};
          if (decimal->is_too_big()) {
            throw_error(
                this, Diagnostics::Diagnostic(
                          std::make_unique<Diagnostics::DiagnosticHighlighter>(
                              token_start, decimal->get_token().length(),
                              file.line()),
                          "Immediate literal requires more than 16 bits to "
                          "represent",
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
      DEBUG("TOKEN START {}, TOKEN END {}", token_start, token_end);
      auto &&token{line.substr(token_start, token_end)};
      if (token.empty()) {
        continue;
      }
      if (auto &&t_token{tokenize(token)};
          t_token->token_type() == TokenType::IMMEDIATE &&
          t_token->is_too_big()) {
        throw_error(
            this,
            Diagnostics::Diagnostic(
                std::make_unique<Diagnostics::DiagnosticHighlighter>(
                    token_start, t_token->get_token().length(), file.line()),
                "Immediate literal requires more than 16 bits to represent",
                file.name(), file.position().line()));
        l_tokens.emplace_back(std::make_unique<Token::Token>(
            std::string(token), file.position().line(),
            file.position().column(), file.name()));
      } else {
        DEBUG("FOUND THIS {}", *t_token);
        l_tokens.emplace_back(std::move(t_token));
      }
      terminator = '\0';
    }
  }

  return l_tokens;
}

} // namespace Lexer::Tokenizer
