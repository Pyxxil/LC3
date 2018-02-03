#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include "cxxopts.hpp"
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#include "spdlog/spdlog.h"

#include "Lexer/Lexer.hpp"
#include "Lexer/Parser.hpp"

// Options:
//  - Change Verbosity
//  - Print Help
//  - Print AST (of sorts)
//  - Stop on first error
//  - Report Warnings as Errors

class Assembler {
public:
  Assembler(int argc, char **argv)
      : argumentCount(argc), argumentValues(argv),
        options("LC3AS", "An assembler for the LC3 Assembly language") {
    parse_options();
  }

  void parse_options() {
    options.positional_help("<assembly files>");
    options.add_options()("h,help", "Print this help message")(
        "print-ast",
        "Print the syntax tree for how the assembler parsed the code")(
        "e,error",
        "Treat warnings as errors")("files", "The files to assemble",
                                    cxxopts::value<std::vector<std::string>>())(
        "q,quiet", "Lessen the output to virtually nothing")(
        "no-warn", "Don't display warnings (if --error is specified, then this "
                   "option basically has no effect");
  }

  int assemble() {
    using Lexer::File;
    using Lexer::Lexer;
    using Parser::Parser;
    using namespace Notification;

    try {
      options.parse_positional("files");
      auto parsed = options.parse(argumentCount, argumentValues);

      if (parsed["help"].as<bool>()) {
        std::cout << options.help() << '\n';
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
      const bool weShouldPrintAST = parsed["print-ast"].as<bool>();
      const bool weShouldTreatWarningsAsErrors = parsed["error"].as<bool>();
      const bool weShouldntDisplayWarnings = parsed["no-warn"].as<bool>();
      const bool weShouldBeQuiet = parsed["quiet"].as<bool>();

      if (!weShouldBeQuiet) {
        Callback errors("LC3AS",
                        [&errors](auto &&, auto &&diagnostic) {
                          errors.error("Error: {}", diagnostic);
                        },
                        false, false);

        Callback warnings(
            "LC3AS",
            [&warnings, weShouldTreatWarningsAsErrors,
             weShouldntDisplayWarnings](auto &&, auto &&diagnostic) {
              if (weShouldTreatWarningsAsErrors) {
                warnings.error("Error: {}", diagnostic);
              } else if (!weShouldntDisplayWarnings) {
                warnings.warn("Warning: {}", diagnostic);
              }
            },
            false, false);
        error_notifications << errors;
        warning_notifications << warnings;
      }

      int retValue = 0;

      for (auto &file : files) {
        Lexer lexer(File{file}, weShouldTreatWarningsAsErrors);

        lexer.lex();

        if (weShouldPrintAST) {
          ast_console->info("{}\n", lexer);
        }

        warning_notifications.notify_all_and_clear();

        if (lexer.okay()) {
          Parser parser(std::move(lexer.tokens));
        } else {
          retValue = 1;
          error_notifications.notify_all_and_clear();
        }
      }

      return retValue;
    } catch (const cxxopts::OptionParseException &e) {
      std::cerr << e.what() << '\n' << options.help();
      return 1;
    }
  }

private:
  int argumentCount;
  char **argumentValues;
  cxxopts::Options options;
};

#endif