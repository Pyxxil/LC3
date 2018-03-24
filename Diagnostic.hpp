#ifndef DIAGNOSTIC_HPP
#define DIAGNOSTIC_HPP

#include <iostream>
#include <memory>
#include <utility>

namespace Diagnostics {

class DiagnosticHighlighter {
public:
  DiagnosticHighlighter(std::size_t tColumn, std::size_t tLength,
                        const std::string &tContext)
      : mContext(tContext), mColumn(tColumn), mLength(tLength) {
    if (!mContext.empty()) {
      mHighlighter = std::string(column(), ' ');
      mHighlighter.push_back('^');
      if (length() > 0) {
        mHighlighter += std::string(length(), '~');
      }
    }
  }

  std::size_t column() const { return mColumn; }
  std::size_t length() const { return mLength; }

  template <typename OStream>
  friend OStream &operator<<(OStream &os,
                             const DiagnosticHighlighter &Highlighter) {
    return os << Highlighter.mContext << '\n' << Highlighter.mHighlighter;
  }

protected:
  std::string mHighlighter{};
  std::string mContext;
  std::size_t mColumn;
  std::size_t mLength;
};

class Diagnostic {
public:
  Diagnostic() = default;
  explicit Diagnostic(std::unique_ptr<DiagnosticHighlighter> highlighter,
                      std::string t_message, const std::string &file_name,
                      std::size_t at_line)
      : mHighlighter(std::move(highlighter)), message(std::move(t_message)),
        file(file_name), line(at_line) {}

  Diagnostic(const Diagnostic &) = default;
  Diagnostic(Diagnostic &&) noexcept = default;

  Diagnostic &operator=(const Diagnostic &) = default;
  Diagnostic &operator=(Diagnostic &&) noexcept = default;

  template <typename OStream>
  friend OStream &operator<<(OStream &os, const Diagnostic &diagnostic) {
    os << diagnostic.file << ':' << diagnostic.line << ':'
       << diagnostic.mHighlighter->column() << ": " << diagnostic.message
       << '\n'
       << *diagnostic.mHighlighter;
    return os;
  }

private:
  std::unique_ptr<DiagnosticHighlighter> mHighlighter;
  std::string message;
  std::string file;
  std::size_t line;
};
} // namespace Diagnostics

#endif