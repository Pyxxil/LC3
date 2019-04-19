#include "String.hpp"

namespace Lexer::Token {

String::String(std::string s, size_t t_line, size_t t_column,
               const std::string &t_file)
    : Token(std::move(s), t_line, t_column, t_file, Requirements()) {
  std::string real{};
  for (size_t idx = 0; idx < token.length(); ++idx) {
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

const std::string &String::get_token() const {
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

} // namespace Lexer::Token