#include "Lexer.hpp"

#include "Match.hpp"

std::map<std::string, const Lexer::File &> Lexer::open_files{};

void throw_error(Lexer::Tokenizer::Tokenizer *tokenizer,
                 Diagnostics::Diagnostic error) {
  Notification::error_notifications << std::move(error);
  tokenizer->lexer().error();
}

void throw_warning(Lexer::Tokenizer::Tokenizer *tokenizer,
                   Diagnostics::Diagnostic warning) {
  Notification::warning_notifications << std::move(warning);
  tokenizer->lexer().warn();
}

namespace {

using namespace Lexer;

bool file_exists(const std::string &file) {
  return Algorithm::any(open_files.cbegin(), open_files.cend(),
                        [&file](auto &&f) { return f.first == file; });
}

const Match requires_zero = Match(TokenType::LABEL) | Match(TokenType::RET) |
                            Match(TokenType::END) | Match(TokenType::HALT) |
                            Match(TokenType::PUTS) | Match(TokenType::OUT) |
                            Match(TokenType::GETC) | Match(TokenType::IN) |
                            Match(TokenType::PUTC) | Match(TokenType::RTI)
#ifdef KEEP_COMMENTS
                            | Match(TokenType::COMMENT)
#endif
    ;
} // namespace

namespace Lexer {

Lexer::Lexer() : m_tokenizer(*this, m_file) {}

Lexer::Lexer(File file, bool warnings_are_errors)
    : m_file(std::move(file)), from_file(true),
      treat_warnings_as_errors(warnings_are_errors),
      m_tokenizer(*this, m_file) {
  if (file_exists(m_file.name())) {
    Notification::error_notifications << Diagnostics::Diagnostic(
        std::make_unique<Diagnostics::DiagnosticHighlighter>(),
        "File is already open (probable recursive include)", m_file.name());
    error();
    return;
  }

  if (m_file.is_failure()) {
    Notification::error_notifications << Diagnostics::Diagnostic(
        std::make_unique<Diagnostics::DiagnosticHighlighter>(),
        "Unable to open file", m_file.name());
    error();
    return;
  }

  open_files.emplace(m_file.name(), m_file);

  while (m_file.next_line()) {
    auto &&l_tokens = m_tokenizer.tokenize_line(Line(m_file.line()));
    tokens.insert(std::end(tokens),
                  std::make_move_iterator(std::begin(l_tokens)),
                  std::make_move_iterator(std::end(l_tokens)));
  }
}

Lexer::~Lexer() {
  if (const auto it = open_files.find(m_file.name()); it != open_files.end()) {
    open_files.erase(it);
  }
}

void Lexer::each(const std::function<void(const Token::Token &)> &f) const {
  Algorithm::each(tokens.cbegin(), tokens.cend(), [&f](auto &&t) { f(*t); });
}

void Lexer::error() {

  m_okay = false;
  ++error_count;
}

void Lexer::warn() {
  if (treat_warnings_as_errors) {
    error();
  }
}

void Lexer::lex() {
  for (idx = 0; idx < tokens.size(); ++idx) {
    auto &token = *(tokens[idx]);
    const auto &requirements = token.requirements();

    for (auto &&tok : requirements.consume(tokens, idx, m_file)) {
      token.add_operand(std::move(tok));
    }

    if (const auto tokens_removed = tokens[idx]->operands().size();
        tokens_removed > 0) {
      const auto begin = tokens.cbegin() + idx + 1;
      tokens.erase(begin, begin + tokens_removed);
    }

    if (!requirements.are_satisfied()) {
      // No need to push a diagnostic here, as Requirements::consume will have
      // pushed the diagnostic for us.
      error();
      ++idx; // Skip the next token, because otherwise it's luikely that the
      // below else if will error out twice for that token.
    } else if (requirements.count() == 0) {
      if (!(requires_zero & token.token_type())) {
        Notification::error_notifications << Diagnostics::Diagnostic(
            std::make_unique<Diagnostics::DiagnosticHighlighter>(
                token.column(), token.get_token().length(),
                m_file.line(token.line() - 1)),
            fmt::format("Expected Label, Instruction, or Directive, but "
                        "found '{}' (type {}) instead.",
                        token, token.token_type()),
            token.file(), token.line());
        error();
      }
#ifdef ADDONS
    } else if (token.token_type() == TokenType::INCLUDE) {
      has_includes = true;
#endif
    }
  }

#ifdef ADDONS
  if (has_includes) {
    std::vector<std::unique_ptr<Token::Token>> _tokens;
    _tokens.reserve(tokens.capacity());
    for (auto &&token : tokens) {
      if (token->token_type() == TokenType::INCLUDE) {
        const auto &file_name = token->operands().front()->get_token();
        Lexer lexer(File{file_name.substr(1, file_name.length() - 2)});
        lexer.lex();

        // it = tokens.erase(it);
        if (m_okay = lexer.is_okay(); m_okay) {
          // TODO: This annoyingly means we will look over every token inside
          // the new tokens as well, which is done by the child lexer...
          _tokens.insert(std::end(_tokens),
                         std::make_move_iterator(std::begin(lexer.tokens)),
                         std::make_move_iterator(std::end(lexer.tokens)));
        }
      } else {
        _tokens.emplace_back(std::move(token));
      }
    }
    tokens = std::move(_tokens);
  }
#endif
}

std::vector<std::unique_ptr<Token::Token>> Lexer::consume_range(size_t begin,
                                                                size_t end) {
  std::vector<std::unique_ptr<Token::Token>> consumed;

  if (idx + begin > tokens.size()) {
    return consumed;
  }

  const auto _end = Algorithm::min(end, tokens.size() - idx);

  for (std::size_t i = 0; i < _end; ++i) {
    consumed.emplace_back(std::move(tokens[idx++]));
  }

  return consumed;
}

std::vector<std::unique_ptr<Token::Token>> Lexer::consume(std::size_t count) {
  if (idx + count > tokens.size()) {
    // Should this be handled here, or inside the caller?
    return {};
  }

  return consume_range(count, count);
}

void Lexer::set_file(File file) {
  m_file = std::move(file);
  from_file = true;
}

void Lexer::unset_file() { from_file = false; }

template <typename T, typename> Lexer &Lexer::operator<<(T s) {
  tokens.emplace_back(new T(s));
  return *this;
}

template <> Lexer &Lexer::operator<<<std::string_view>(std::string_view s) {
  auto &&l_tokens = m_tokenizer.tokenize_line(Line(s));
  tokens.insert(std::end(tokens), std::make_move_iterator(std::begin(l_tokens)),
                std::make_move_iterator(std::end(l_tokens)));
  return *this;
}

template <> Lexer &Lexer::operator<<<std::string>(std::string s) {
  auto &&l_tokens = m_tokenizer.tokenize_line(Line(s));
  tokens.insert(std::end(tokens), std::make_move_iterator(std::begin(l_tokens)),
                std::make_move_iterator(std::end(l_tokens)));
  return *this;
}

template <> Lexer &Lexer::operator<<<File>(File file) {
  set_file(std::move(file));

  while (m_file.next_line()) {
    *this << m_file.line();
  }

  unset_file();
  return *this;
}

} // namespace Lexer