#ifndef NOTIFIER_HPP
#define NOTIFIER_HPP

#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

#include "Debug.hpp"

#include "Callback.hpp"

static const std::string Anonymous_Callback{ "Anonymous Callback" };

namespace Notification {

enum class NOTIFY_EVENT
{
  DIAGNOSTIC,
};

template<NOTIFY_EVENT T>
class Notifier
{
public:
  explicit Notifier<T>(Notification::Callback c)
    : callback(std::move(c))
  {}
  explicit Notifier<T>(Notification::Callback_Func_t f)
    : callback(Anonymous_Callback, std::move(f))
  {}

  Notifier<T>(const Notifier<T>& other) = default;
  Notifier<T>(Notifier<T>&& other) noexcept = default;

  Notifier<T>& operator=(const Notifier<T>& other) = default;
  Notifier<T>& operator=(Notifier<T>&& other) noexcept = default;

  ~Notifier<T>() = default;

  void notify() const;

private:
  Notification::Callback callback;
};

template<NOTIFY_EVENT T>
class Notification_Wrapper
{
public:
  Notification_Wrapper<T>() = default;

  Notification_Wrapper<T>(const Notification_Wrapper<T>&) = default;
  Notification_Wrapper<T>(Notification_Wrapper<T>&&) noexcept = default;

  Notification_Wrapper<T>& operator=(const Notification_Wrapper<T>&) = default;
  Notification_Wrapper<T>& operator=(Notification_Wrapper<T>&&) noexcept =
    default;

  ~Notification_Wrapper() = default;

  Notification_Wrapper& operator<<(Notification::Notifier<T> t)
  {
    notifications.emplace_back(t);
    return *this;
  }

  void for_each(std::function<void(const Notification::Notifier<T>&)> f) const
  {
    for (auto&& notification : notifications) {
      f(notification);
    }
  }

  void notify_all()
  {
    for_each([](auto&& n) { n.notify(); });
  }

  void notify_all_and_clear()
  {
    notify_all();
    notifications.clear();
  }

private:
  std::vector<Notification::Notifier<T>> notifications{};
};

template<>
void
Notifier<NOTIFY_EVENT::DIAGNOSTIC>::notify() const
{
  callback();
}

template<NOTIFY_EVENT T>
using Notification_t = Notification_Wrapper<T>;

Notification_t<NOTIFY_EVENT::DIAGNOSTIC> diagnostic_notifications;

template<NOTIFY_EVENT T>
void add_notification(Notifier<T>);

void
add_notification(Notifier<NOTIFY_EVENT::DIAGNOSTIC> t)
{
  diagnostic_notifications << std::move(t);
}

template<Notification::NOTIFY_EVENT T>
void
add_callback(Notification::Callback c)
{
  Notification::add_notification(Notification::Notifier<T>(std::move(c)));
}
}

#endif