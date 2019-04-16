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
class Lexer;
}

namespace Lexer::Tokenizer {

class Tokenizer {
public:
  Tokenizer(Lexer &t_lexer, File &t_file);

  std::unique_ptr<Token::Token> tokenize_immediate(const std::string_view &s);

  std::unique_ptr<Token::Token> tokenize_directive(const std::string_view &s);

  /*! Tokenize a single word
   *
   * @param s The word to tokenize
   * @return The token the word corresponds to
   */
  std::unique_ptr<Token::Token> tokenize(const std::string_view &s);

  void extraneous(char character);

  std::vector<std::unique_ptr<Token::Token>> tokenize_line(Line line);

  Lexer &lexer() const { return m_lexer; }

private:
  Lexer &m_lexer;
  File &file;
};

} // namespace Lexer::Tokenizer

extern void throw_error(Lexer::Tokenizer::Tokenizer *, Diagnostics::Diagnostic);
extern void throw_warning(Lexer::Tokenizer::Tokenizer *,
                          Diagnostics::Diagnostic);

#endif
