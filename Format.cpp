#include "Format.hpp"

int main(int argc, char **argv) {
  Formatter::Formatter formatter(argc, argv);
  return formatter.format();
}