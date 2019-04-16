#include "Notifier.hpp"

size_t Notification::count() {
  return diagnostic_notifications.count() + error_notifications.count() +
         warning_notifications.count();
}

Notification::Notification_Wrapper<Notification::NOTIFY_EVENT::DIAGNOSTIC>
    Notification::diagnostic_notifications;
Notification::Notification_Wrapper<Notification::NOTIFY_EVENT::ERROR>
    Notification::error_notifications;
Notification::Notification_Wrapper<Notification::NOTIFY_EVENT::WARNING>
    Notification::warning_notifications;
