#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"
#include "Tokens.hpp"

namespace Parser {
struct Label {
  Label(const std::string &name, size_t address);

  std::string mName;
  size_t mAddress;
};

class Parser {
public:
  Parser(std::vector<std::unique_ptr<Lexer::Token::Token>> tokens)
      : mTokens(std::move(tokens)) {}

  void parse() {
    uint16_t currentAddress{0};
    bool originSeen{false};
    bool endSeen{false};

    if (mTokens.front()->tokenType() != Lexer::TokenType::ORIG) {
      error();
      return;
    }

    for (auto &token : mTokens) {
      switch (token->tokenType()) {
#ifdef ADDONS
      case Lexer::TokenType::INCLUDE:
        break;
#endif
      case Lexer::TokenType::LABEL:
        break;
      case Lexer::TokenType::ORIG:
        if (originSeen) {
          error();
        } else {
          currentAddress =
              static_cast<uint16_t>(static_cast<Lexer::Token::Immediate *>(
                                        token->operands().front().get())
                                        ->value());
          originSeen = true;
        }
        break;
      case Lexer::TokenType::END:
        break;
      default:
        if (!originSeen) {
          error();
        } else if (endSeen) {
          Notification::error_notifications << Diagnostics::Diagnostic(
              std::make_unique<Diagnostics::DiagnosticHighlighter>(
                  token->column(), token->getToken().length(), ""),
              "Extra .END directive found.", token->file(), token->line());
        } else {
          word memoryRequired = token->memoryRequired();
          if (memoryRequired == -1) {
            error();
          } else if (memoryRequired > 0) {
            currentAddress += static_cast<uint16_t>(memoryRequired);
          }
        }
        break;
      }
    }
  }

  void error() { ++errorCount; }
  void warning() {}

  const auto &tokens() const { return mTokens; }

  bool isOkay() const { return errorCount == 0; }

private:
  std::vector<std::unique_ptr<Lexer::Token::Token>> mTokens;
  size_t errorCount{0};
}; // namespace Parser
} // namespace Parser

#endif
