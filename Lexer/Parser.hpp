#ifndef PARSER_HPP
#define PARSER_HPP

#include <map>

#include "Lexer.hpp"
#include "Tokens.hpp"

namespace Parser {
class Parser {
public:
  explicit Parser(std::vector<std::unique_ptr<Lexer::Token::Token>> tokens)
      : m_tokens(std::move(tokens)) {}

  void parse() {
    uint16_t current_address{0};
    bool origin_seen{false};
    bool end_seen{false};

    if (m_tokens.front()->token_type() != Lexer::TokenType::ORIG) {
      error();
      return;
    }

    for (auto &token : m_tokens) {
      switch (token->token_type()) {
      case Lexer::TokenType::LABEL: {
        if (!origin_seen) {
          error();
          break;
        } else if (end_seen) {
          Notification::warning_notifications << Diagnostics::Diagnostic(
              std::make_unique<Diagnostics::DiagnosticHighlighter>(
                  token->column(), token->get_token().length(), std::string{}),
              "Label found after .END directive, ignoring.", token->file(),
              token->line());
          break;
        }

        for (auto &&[_, symbol] : m_symbols) {
          if (symbol.address() == current_address) {
            Notification::error_notifications << Diagnostics::Diagnostic(
                std::make_unique<Diagnostics::DiagnosticHighlighter>(
                    token->column(), token->get_token().length(),
                    std::string{}),
                "Multiple labels found for address", token->file(),
                token->line());
            Notification::error_notifications << Diagnostics::Diagnostic(
                std::make_unique<Diagnostics::DiagnosticHighlighter>(
                    symbol.column(), symbol.name().length(), std::string{}),
                "Previous label found here", symbol.file(), symbol.line());
          }
        }

        auto &&[symbol, inserted] = m_symbols.try_emplace(
            token->get_token(),
            Lexer::Symbol(token->get_token(), current_address, token->file(),
                          token->column(), token->line()));

        if (!inserted) {
          // TODO: Fix the way these are handled. At the moment, any errors
          // TODO: thrown here from labels that have been included (from the
          // TODO: same file) won't actually be useful due to the fact that it
          // TODO: doesn't tell the user where the .include was found.
          auto &&sym = symbol->second;
          Notification::error_notifications
              << Diagnostics::Diagnostic(
                     std::make_unique<Diagnostics::DiagnosticHighlighter>(
                         token->column(), token->get_token().length(),
                         std::string{}),
                     "Multiple definitions of label", token->file(),
                     token->line())
              << Diagnostics::Diagnostic(
                     std::make_unique<Diagnostics::DiagnosticHighlighter>(
                         sym.column(), sym.name().length(), std::string{}),
                     "Previous definition found here", sym.file(), sym.line());
        } else {
          longest_symbol_length =
              std::max(longest_symbol_length,
                       static_cast<int>(token->get_token().length()));
        }
        break;
      }
      case Lexer::TokenType::ORIG:
        if (origin_seen) {
          error();
        } else {
          current_address =
              static_cast<uint16_t>(static_cast<Lexer::Token::Immediate *>(
                                        token->operands().front().get())
                                        ->value());
          origin_seen = true;
        }
        break;
      case Lexer::TokenType::END:
        end_seen = true;
        break;
      default:
        if (!origin_seen) {
          error();
        } else if (end_seen) {
          Notification::warning_notifications << Diagnostics::Diagnostic(
              std::make_unique<Diagnostics::DiagnosticHighlighter>(
                  token->column(), token->get_token().length(), std::string{}),
              "Extra .END directive found.", token->file(), token->line());
        } else {
          if (const word memory_required = token->memory_required();
              memory_required == -1) {
            error();
          } else if (memory_required > 0) {
            current_address += static_cast<uint16_t>(memory_required);
          }
        }
        break;
      }
    }
  }

  void error() { ++error_count; }
  void warning() {}

  const auto &tokens() const { return m_tokens; }
  const auto &symbols() const { return m_symbols; }

  auto is_okay() const { return error_count == 0; }

private:
  std::vector<std::unique_ptr<Lexer::Token::Token>> m_tokens{};
  std::map<std::string, Lexer::Symbol> m_symbols{};
  size_t error_count{0};
  int longest_symbol_length{20};
}; // namespace Parser
} // namespace Parser

#endif
