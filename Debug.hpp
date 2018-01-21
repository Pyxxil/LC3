#ifndef DEBUG_HPP
#define DEBUG_HPP

#ifndef NDEBUG
#include "spdlog/spdlog.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "spdlog/fmt/ostr.h"
#pragma GCC diagnostic pop

namespace Debug {
auto debug_console = spdlog::stdout_color_st("debug");
}

#define DEBUG_HELPER(fmt, ...) Debug::debug_console->debug(fmt, __VA_ARGS__)
#define DEBUG(...) DEBUG_HELPER(__VA_ARGS__)

#else
#define DEBUG(...)
#endif

#endif