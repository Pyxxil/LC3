#ifndef ASSEMBLEDTOKEN_HPP
#define ASSEMBLEDTOKEN_HPP

#include <string>

namespace Lexer::Token {

class AssembledToken {
public:
  AssembledToken() = default;
  AssembledToken(uint16_t t_bin, std::string t_lst_str);
  AssembledToken(const AssembledToken &) = default;
  AssembledToken(AssembledToken &&) noexcept = default;
  AssembledToken &operator=(const AssembledToken &) = default;
  AssembledToken &operator=(AssembledToken &&) noexcept = default;
  ~AssembledToken() = default;

  [[nodiscard]] const std::string &lst_str() const;
  [[nodiscard]] std::pair<uint8_t, uint8_t> binary() const;

private:
  uint16_t m_bin{};
  std::string m_lst_str;
};

} // namespace Lexer::Token

#endif
