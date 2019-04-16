#include "AssembledToken.hpp"

namespace Lexer::Token {

AssembledToken::AssembledToken(uint16_t t_bin, std::string t_lst_str)
    : m_bin{t_bin}, m_lst_str{std::move(t_lst_str)} {}

std::pair<uint8_t, uint8_t> AssembledToken::binary() const {
  return std::make_pair(static_cast<uint8_t>(m_bin >> 8 & 0xFF),
                        static_cast<uint8_t>(m_bin & 0xFF));
}

const std::string &AssembledToken::lst_str() const { return m_lst_str; }

} // namespace Lexer::Token