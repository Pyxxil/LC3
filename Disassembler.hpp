#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP

#include <fstream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include "cxxopts.hpp"
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#include "fmt/printf.h"

#include "Callback.hpp"
#include "Diagnostic.hpp"
#include "Notifier.hpp"

class Disassembler {
public:
  Disassembler(int argc, char **argv)
      : argument_count{argc}, argument_values{argv},
        options("LC3DIS", "A disassembler for object files made for the LC3") {
    parse_options();
  }

  void parse_options() {
    options.positional_help("<object files>");
    options.add_options()("h,help", "Print this help message")(
        "files", "The files to assemble",
        cxxopts::value<std::vector<std::string>>())(
        "q,quiet", "Lessen the output to virtually nothing")(
        "no-color", "Alias for no-colour")("no-colour",
                                           "Remove colour from the output");
  }

  int disassemble() {
    bool we_should_be_quiet;
    bool we_shouldnt_use_colours;

    std::vector<std::string> files;

    try {
      options.parse_positional("files");
      auto parsed = options.parse(argument_count, argument_values);

      if (parsed["help"].as<bool>()) {
        std::cout << options.help() << '\n';
        return 0;
      }

      if (0 == parsed.count("files")) {
        std::cerr << "No input files\n";
        return 1;
      }

      files = std::move(parsed["files"].as<std::vector<std::string>>());

      we_should_be_quiet = parsed["quiet"].as<bool>();
      we_shouldnt_use_colours =
          parsed["no-colour"].as<bool>() || parsed["no-color"].as<bool>();
    } catch (const cxxopts::OptionParseException &e) {
      std::cerr << e.what() << '\n' << options.help();
      return 1;
    }

    if (!we_should_be_quiet) {
      Notification::Callback errors(
          "LC3DIS",
          [&errors](auto &&, auto &&diagnostic) {
            errors.error(
                "{}: {}",
                fmt::format("{0:s}{1:s}{2:s}",
                            Console::Colour(Console::FOREGROUND_COLOUR::RED),
                            "Error", Console::reset),
                diagnostic);
          },
          false, false);

      Notification::error_notifications << errors;
    }

    int ret_value = 0;

    for (auto &&file : files) {
      // TODO: Features
      //    - Try to find an LST/SYM file and parse it?
      //    - Potentially determine some symbols by ourselves if we can't find a
      //      sym file?

      Notification::error_notifications.notify_all_and_clear();

      std::ifstream object_file(file, std::ios::binary);
      if (!object_file.is_open() || object_file.fail()) {
        Notification::error_notifications << Diagnostics::Diagnostic(
            std::make_unique<Diagnostics::DiagnosticHighlighter>(),
            "File cannot be opened", file, 0);

        ret_value = 1;
        continue;
      }

      const auto file_name = file.substr(0, file.rfind('.'));
      const auto asm_file_name = file_name + ".asm";

      auto obj_file = std::ofstream(asm_file_name);
    }

    Notification::error_notifications.notify_all_and_clear();

    return ret_value;
  }

private:
  int argument_count;
  char **argument_values;
  cxxopts::Options options;
};

#endif // DISASSEMBLER.HPP