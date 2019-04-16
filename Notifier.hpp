#ifndef NOTIFIER_HPP
#define NOTIFIER_HPP

#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

#include "Debug.hpp"

#include "Algorithm.hpp"
#include "Callback.hpp"
#include "Diagnostic.hpp"

static const char Anonymous_Callback[] = "Anonymous Callback";

namespace Notification {

enum class NOTIFY_EVENT {
  DIAGNOSTIC,
  ERROR,
  WARNING,
};

template <NOTIFY_EVENT T> class Notification_Wrapper {
public:
  Notification_Wrapper<T>() = default;

  Notification_Wrapper<T>(const Notification_Wrapper<T> &) = default;
  Notification_Wrapper<T>(Notification_Wrapper<T> &&) noexcept = default;

  Notification_Wrapper<T> &operator=(const Notification_Wrapper<T> &) = default;
  Notification_Wrapper<T> &
  operator=(Notification_Wrapper<T> &&) noexcept = default;

  ~Notification_Wrapper() = default;

  /*! Add a diagnostic to the queue.
   *
   */
  Notification_Wrapper &operator<<(Diagnostics::Diagnostic diagnostic) {
    emplace(std::move(diagnostic));
    return *this;
  }

  /*! Add a callback to the queue.
   *
   * All callbacks will, before being added, have the past diagnostics
   * thrown at them so they can catch up.
   *
   * @param callback The callback to add
   */
  Notification_Wrapper &operator<<(Callback callback) {
    if (callback.wants_previous()) {
      each(callback);
    }

    callbacks.emplace_back(std::move(callback));
    return *this;
  }

  void each(std::function<void(const Diagnostics::Diagnostic &)> f) const {
    Algorithm::each(diagnostics.cbegin(), diagnostics.cend(), f);
  }

  void notify_each() const {
    Algorithm::each(callbacks.cbegin(), callbacks.cend(),
                    [this](const auto &cb) { each(cb); });
  }

  void notify_all(bool force_updates = false) {
    Algorithm::each(callbacks.cbegin(), callbacks.cend(),
                    [this, force_updates](const auto &cb) {
                      if (force_updates || cb.wants_updates()) {
                        cb(diagnostics.back());
                      }
                    });
  }

  size_t count() const { return diagnostics.size(); }

  void notify_all_and_clear() {
    notify_each();
    diagnostics.clear();
  }

private:
  void emplace(Diagnostics::Diagnostic diagnostic) {
    diagnostics.emplace_back(std::move(diagnostic));
  }

  std::vector<Notification::Callback> callbacks{};
  std::vector<Diagnostics::Diagnostic> diagnostics{};
};

extern Notification_Wrapper<NOTIFY_EVENT::DIAGNOSTIC> diagnostic_notifications;
extern Notification_Wrapper<NOTIFY_EVENT::ERROR> error_notifications;
extern Notification_Wrapper<NOTIFY_EVENT::WARNING> warning_notifications;

size_t count();

} // namespace Notification

#endif
