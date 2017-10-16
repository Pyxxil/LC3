#ifndef CALLBACK_HPP
#define CALLBACK_HPP

#include <functional>
#include <string>
#include <vector>

namespace Notification {

using Name_t = std::string;
using Callback_Func_t = std::function<void(const Name_t&)>;

class Callback
{
public:
  Callback(Name_t t_name, Callback_Func_t f)
    : callback(std::make_pair<Name_t, Callback_Func_t>(std::move(t_name),
                                                       std::move(f)))
  {}
  Callback(const Callback& c) = default;
  Callback(Callback&& c) noexcept = default;

  Callback& operator=(const Callback& c) = default;
  Callback& operator=(Callback&& c) noexcept = default;

  ~Callback() = default;

  void operator()() const { callback.second(name()); }

  const Name_t& name() const { return callback.first; }

private:
  std::pair<Name_t, Callback_Func_t> callback;
};
}

#endif
