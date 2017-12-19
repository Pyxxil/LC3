#ifndef NOTIFIER_HPP
#define NOTIFIER_HPP

#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

#include "Debug.hpp"

#include "Callback.hpp"
#include "Diagnostic.hpp"

static const char Anonymous_Callback[] = "Anonymous Callback";

namespace Notification {

enum class NOTIFY_EVENT {
  DIAGNOSTIC,
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
      for_each(callback);
    }

    callbacks.emplace_back(std::move(callback));
    return *this;
  }

  void for_each(std::function<void(const Diagnostics::Diagnostic &)> f) const {
    for (auto &&diagnostic : diagnostics) {
      f(diagnostic);
    }
  }

  void notify_all(bool force_updates = false) {
    if (force_updates) {
      for (const auto &callback : callbacks) {
        callback(diagnostics.back());
      }
    } else {
      for (const auto &callback : callbacks) {
        if (callback.wants_updates()) {
          callback(diagnostics.back());
        }
      }
    }
  }

  void notify_all_and_clear() {
    notify_all(true);
    callbacks.clear();
    diagnostics.clear();
  }

private:
  void emplace(Diagnostics::Diagnostic diagnostic) {
    diagnostics.emplace_back(std::move(diagnostic));
    notify_all();
  }

  std::vector<Notification::Callback> callbacks{};
  std::vector<Diagnostics::Diagnostic> diagnostics{};
};

Notification_Wrapper<NOTIFY_EVENT::DIAGNOSTIC> diagnostic_notifications;

} // namespace Notification

#endif