#ifndef DIAGNOSTIC_HPP
#define DIAGNOSTIC_HPP

#include <iostream>
#include <memory>
#include <utility>

namespace Diagnostics {

class DiagnosticHighlighter {
public:
  DiagnosticHighlighter(std::size_t at_column, std::size_t t_length,
                        const std::string &t_context)
      : context(t_context), m_column(at_column), length(t_length) {
    if (!t_context.empty()) {
      highlighter = std::string(m_column, ' ');
      highlighter.push_back('^');
      highlighter += std::string(length, '~');
      highlighter += std::string(context.length() - (length + m_column), ' ');
    }
  }

  std::size_t column() const { return m_column; }

  template <typename OStream>
  friend OStream &operator<<(OStream &os,
                             const DiagnosticHighlighter &Highlighter) {
    return os << Highlighter.context << '\n' << Highlighter.highlighter;
  }

protected:
  std::string highlighter{};
  std::string context;
  std::size_t line;
  std::size_t m_column;
  std::size_t length;
};

class Diagnostic {
public:
  Diagnostic() = default;
  explicit Diagnostic(std::unique_ptr<DiagnosticHighlighter> highlighter,
                      std::string t_message, const std::string &file_name,
                      std::size_t at_line)
      : m_highlighter(std::move(highlighter)), message(std::move(t_message)),
        file(file_name), line(at_line) {}

  Diagnostic(const Diagnostic &) = default;
  Diagnostic(Diagnostic &&) noexcept = default;

  Diagnostic &operator=(const Diagnostic &) = default;
  Diagnostic &operator=(Diagnostic &&) noexcept = default;

  template <typename OStream>
  friend OStream &operator<<(OStream &os, const Diagnostic &diagnostic) {
    os << diagnostic.file << ':' << diagnostic.line << ':'
       << diagnostic.m_highlighter->column() << ": " << diagnostic.message
       << '\n'
       << *diagnostic.m_highlighter;
    return os;
  }

private:
  std::unique_ptr<DiagnosticHighlighter> m_highlighter;
  std::string message;
  std::string file;
  std::size_t line;
};
} // namespace Diagnostics

#endif