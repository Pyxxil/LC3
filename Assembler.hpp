#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "cxxopts.hpp"
#include "spdlog/spdlog.h"

#include "Lexer/Lexer.hpp"

// Options:
//  - Change Verbosity
//  - Print Help
//  - Print AST (of sorts)
//  - Stop on first error
//  - Report Warnings as Errors

class Assembler {
public:
  Assembler(int argc, char **argv)
      : argument_count(argc), argument_values(argv),
        options("LC3AS", "An assembler for the LC3 Assembly language") {
    parse_options();
  }

  void parse_options() {
    options.positional_help("<assembly files>");
    options.add_options()("h,help", "Print this help message")(
        "print-ast",
        "Print the syntx tree for how the assembler parsed the code")(
        "e,error",
        "Treat warnings as errors")("files", "The files to assemble",
                                    cxxopts::value<std::vector<std::string>>())(
        "q,quiet", "Lessen the output to virtually nothing")(
        "no-warn", "Don't display warnings (if --error is specified, then this "
                   "option basically has no effect");
  }

  int assemble() {
    try {
      options.parse_positional("files");
      auto parsed = options.parse(argument_count, argument_values);

      if (parsed["help"].as<bool>()) {
        std::cerr << options.help() << '\n';
        return 0;
      }

      if (0 == parsed.count("files")) {
        std::cerr << "No input files\n";
        return 1;
      }

      spdlog::set_pattern("%v");

      auto ast_console = spdlog::stdout_color_mt("ast_console");
      ast_console->set_level(spdlog::level::info);
      ast_console->set_pattern("%v");

      auto &&files = parsed["files"].as<std::vector<std::string>>();
      const bool do_print_ast = parsed["print-ast"].as<bool>();
      const bool warnings_as_errors = parsed["error"].as<bool>();
      const bool dont_display_warnings = parsed["no-warn"].as<bool>();

      if (!parsed["quiet"].as<bool>()) {
        Notification::Callback errors("Test",
                                      [&errors](auto &&, auto &&diagnostic) {
                                        errors.error("Error: {}", diagnostic);
                                      },
                                      false, false);

        Notification::Callback warnings(
            "Test",
            [&warnings, warnings_as_errors,
             dont_display_warnings](auto &&, auto &&diagnostic) {
              if (warnings_as_errors) {
                warnings.error("Error: {}", diagnostic);
              } else if (!dont_display_warnings) {
                warnings.warn("Warning: {}", diagnostic);
              }
            },
            false, false);
        Notification::error_notifications << errors;
        Notification::warning_notifications << warnings;
      }

      for (auto &file : files) {
        Lexer::Lexer lexer(Lexer::File{file}, warnings_as_errors);

        lexer.lex();

        Notification::warning_notifications.notify_for_each();

        if (lexer.okay()) {
          if (do_print_ast) {
            ast_console->info("{}\n", lexer);
          }
        } else {
          Notification::error_notifications.notify_for_each();
        }
      }

      return 0;
    } catch (const cxxopts::OptionParseException &e) {
      std::cerr << e.what() << '\n' << options.help();
      return 1;
    }
  }

private:
  int argument_count;
  char **argument_values;
  bool failed{false};
  cxxopts::Options options;
};

#endif