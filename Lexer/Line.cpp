#include "Line.hpp"

namespace Lexer {

Line::Line(const std::string_view &t_line) : m_line(t_line) {}

const std::array<std::function<bool(const Line &, char)>, 2> Line::ignores{
    [](const Line &, char) -> bool { return true; },
    [](const Line &l, char) -> bool { return l.at(l.index() - 1) != '\\'; }};

} // namespace Lexer