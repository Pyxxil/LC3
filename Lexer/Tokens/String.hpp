#ifndef TOKEN_STRING_HPP
#define TOKEN_STRING_HPP

#include "Token.hpp"

namespace Lexer {
namespace Token {
class String : public Token {
public:
  String(std::string s, size_t tLine, size_t tColumn, const std::string &tFile)
      : Token(std::move(s), tLine, tColumn, tFile, Requirements()) {
    std::string real{};
    for (auto idx = 0; idx < token.length(); ++idx) {
      switch (auto c = token[idx]; c) {
      case '\\':
        if (idx == token.length() - 1) {
          break;
        }
        switch (auto cx = token[idx + 1]; cx) {
        case '"':
          token.erase(idx, 1);
          real.push_back(cx);
          break;
        case 'n':
          token.erase(idx, 1);
          real.push_back('\n');
          break;
        case 't':
          token.erase(idx, 1);
          real.push_back('\t');
          break;
        default:
          real.push_back(cx);
          break;
        }
        break;
      default:
        real.push_back(c);
        break;
      }
    }
    token = std::move(real);
  }

  String(const String &) = default;
  String(String &&) noexcept = default;

  String &operator=(const String &) = default;
  String &operator=(String &&) noexcept = default;

  TokenType tokenType() const final { return STRING; }

  const std::string &trueToken() const { return token; }

  const std::string &getToken() const override {
    if (string.empty()) {
      string.push_back('"');
      for (const auto c : token) {
        switch (c) {
        case '\n':
          string += "\\n";
          break;
        case '\t':
          string += "\\t";
          break;
        default:
          string.push_back(c);
          break;
        }
      }
      string.push_back('"');
    }
    return string;
  }

private:
  mutable std::string string{};
};
} // namespace Token
} // namespace Lexer

#endif
