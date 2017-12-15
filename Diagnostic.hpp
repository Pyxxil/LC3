#ifndef DIAGNOSTIC_HPP
#define DIAGNOSTIC_HPP

namespace Diagnostics {
class Diagnostic
{
public:
  Diagnostic() = default;

  Diagnostic(const Diagnostic&) = default;
  Diagnostic(Diagnostic&&) noexcept = default;

  Diagnostic& operator=(const Diagnostic&) = default;
  Diagnostic& operator=(Diagnostic&&) noexcept = default;

  ~Diagnostic() = default;

private:
};
}

#endif