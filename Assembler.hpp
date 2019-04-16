#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include "cxxopts.hpp"
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

// Options:
//  - Change Verbosity
//  - Stop on first error

class Assembler {
public:
  Assembler(int argc, char **argv);

  void parse_options();

  int assemble();

private:
  int argument_count;
  char **argument_values;
  cxxopts::Options options;
};

#endif
