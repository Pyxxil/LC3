#ifndef CALLBACK_HPP
#define CALLBACK_HPP

#include <functional>
#include <string>
#include <vector>

#include "Diagnostic.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "spdlog/fmt/ostr.h"
#pragma GCC diagnostic pop

#include "spdlog/spdlog.h"

namespace Notification {

using Callback_Name_t = std::string;
using Callback_Func_t = std::function<void(const Callback_Name_t &,
                                           const Diagnostics::Diagnostic &)>;

class CallbackLogger {
public:
  static auto &get() { return logger; }

protected:
  CallbackLogger() {
    // logger->set_level(spdlog::level::info);
    // logger->set_pattern("%v");
  }

  static std::shared_ptr<spdlog::logger> logger;
};

std::shared_ptr<spdlog::logger> CallbackLogger::logger =
    spdlog::stdout_color_st("logger");

class Callback {
public:
  Callback(Callback_Name_t t_name, Callback_Func_t f, bool want_prev = true,
           bool update = true)
      : callback(std::make_pair<Callback_Name_t, Callback_Func_t>(
            std::move(t_name), std::move(f))),
        want_previous(want_prev), update_on_each(update) {}
  Callback(const Callback &c) = default;
  Callback(Callback &&c) noexcept = default;

  Callback &operator=(const Callback &c) = default;
  Callback &operator=(Callback &&c) noexcept = default;

  ~Callback() = default;

  void operator()(const Diagnostics::Diagnostic &diagnostic) const {
    callback.second(name(), diagnostic);
  }

  const Callback_Name_t &name() const { return callback.first; }

  bool wants_previous() const { return want_previous; }
  bool wants_updates() const { return update_on_each; }

  template <typename... Args> void warn(const char *fmt, Args &... args) {
    Notification::CallbackLogger::get()->warn(fmt, args...);
  }

  template <typename... Args> void error(const char *fmt, Args &... args) {
    Notification::CallbackLogger::get()->error(fmt, args...);
  }

private:
  std::pair<Callback_Name_t, Callback_Func_t> callback;
  bool want_previous;
  bool update_on_each;
};
} // namespace Notification

#endif
