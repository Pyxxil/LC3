#ifndef DIAGNOSTIC_HPP
#define DIAGNOSTIC_HPP

#include <iostream>

namespace Diagnostics {
class Diagnostic {
public:
  Diagnostic() = default;
  explicit Diagnostic(std::string t_message) : message(t_message) {}

  Diagnostic(const Diagnostic &) = default;
  Diagnostic(Diagnostic &&) noexcept = default;

  Diagnostic &operator=(const Diagnostic &) = default;
  Diagnostic &operator=(Diagnostic &&) noexcept = default;

  ~Diagnostic() = default;

  friend std::ostream &operator<<(std::ostream &os,
                                  const Diagnostic &diagnostic) {
    (void)diagnostic;
    os << diagnostic.message;
    return os;
  }

private:
  std::string message;
};
} // namespace Diagnostics

#endif