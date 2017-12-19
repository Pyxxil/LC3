#ifndef DEBUG_HPP
#define DEBUG_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "spdlog/spdlog.h"
#pragma GCC diagnostic pop

#include "spdlog/fmt/ostr.h"

namespace Debug {
auto console = spdlog::stdout_color_mt("console");
}

#endif