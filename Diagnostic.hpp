#ifndef DIAGNOSTIC_HPP
#define DIAGNOSTIC_HPP

#include <iostream>
#include <memory>
#include <utility>

#include "Console.hpp"

namespace Diagnostics {

class DiagnosticHighlighter {
public:
  DiagnosticHighlighter() = default;

  DiagnosticHighlighter(size_t tColumn, size_t tLength, std::string tContext)
      : mContext(std::move(tContext)), mColumn(tColumn), mLength(tLength) {
    if (!mContext.empty()) {
      mHighlighter.push_back('^');
      if (length() > 0) {
        m_highlighter += std::string(length() - 1, '~');
      }
    }
  }

  auto column() const -> size_t { return m_column; }
  auto length() const -> size_t { return m_length; }

  template <typename OStream>
  friend OStream &operator<<(OStream &os,
                             const DiagnosticHighlighter &highlighter) {
    using namespace Console;
    return os << highlighter.m_context << '\n'
              << std::string(highlighter.column(), ' ')
              << Colour(FOREGROUND_COLOUR::MAGENTA) << highlighter.m_highlighter
              << reset;
  }

protected:
  std::string m_highlighter{};
  std::string m_context;
  size_t m_column;
  size_t m_length;
};

class Diagnostic {
public:
  Diagnostic() = default;
  explicit Diagnostic(std::unique_ptr<DiagnosticHighlighter> highlighter,
                      std::string t_message, const std::string &file_name,
                      size_t at_line)
      : m_highlighter(std::move(highlighter)), m_message(std::move(t_message)),
        m_file(file_name), m_line(at_line) {}

  Diagnostic(const Diagnostic &) = default;
  Diagnostic(Diagnostic &&) noexcept = default;

  Diagnostic &operator=(const Diagnostic &) = default;
  Diagnostic &operator=(Diagnostic &&) noexcept = default;

  auto line() const { return m_line; }
  auto column() const { return m_highlighter->column(); }
  const auto &message() const { return m_message; }
  const auto &file() const { return m_file; }

  template <typename OStream>
  friend OStream &operator<<(OStream &os, const Diagnostic &diagnostic) {
    using namespace Console;
    return os << Colour(FOREGROUND_COLOUR::GREEN) << diagnostic.file() << reset
              << ':' << diagnostic.line() << ':' << diagnostic.column() << ": "
              << diagnostic.message() << '\n'
              << *diagnostic.m_highlighter;
  }

private:
  std::unique_ptr<DiagnosticHighlighter> m_highlighter;
  std::string m_message;
  std::string m_file;
  size_t m_line;
};
} // namespace Diagnostics

#endif
