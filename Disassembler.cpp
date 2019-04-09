#include "Disassembler.hpp"

auto main(int argc, char **argv) -> int {
  Disassembler disassembler(argc, argv);
  return disassembler.disassemble();
}