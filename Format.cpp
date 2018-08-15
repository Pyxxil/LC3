#include "Format.hpp"

auto main(int argc, char **argv) -> int {
  Formatter::Formatter formatter(argc, argv);
  return formatter.format();
}