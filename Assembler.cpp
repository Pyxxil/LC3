#include "Assembler.hpp"

int main(int argc, char **argv) {
  Assembler assembler(argc, argv);
  return assembler.assemble();
}