#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <fstream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include "cxxopts.hpp"
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#include "fmt/printf.h"

#include "Console.hpp"
#include "Lexer/Lexer.hpp"
#include "Lexer/Parser.hpp"

// Options:
//  - Change Verbosity
//  - Stop on first error

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
        "no-warn",
        "Don't display warnings (if --error is specified, then this "
        "option basically has no effect")("no-color", "Alias for no-colour")(
        "no-colour", "Remove colour from the output");
  }

  int assemble() {
    using Lexer::File;
    using Lexer::Lexer;
    using Parser::Parser;
    using namespace Notification;

    bool weShouldPrintAST;
    bool weShouldTreatWarningsAsErrors;
    bool weShouldntDisplayWarnings;
    bool weShouldBeQuiet;
    bool weShouldntUseColours;

    std::vector<std::string> files;

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

      files = std::move(parsed["files"].as<std::vector<std::string>>());

      weShouldPrintAST = parsed["print-ast"].as<bool>();
      weShouldTreatWarningsAsErrors = parsed["error"].as<bool>();
      weShouldntDisplayWarnings = parsed["no-warn"].as<bool>();
      weShouldBeQuiet = parsed["quiet"].as<bool>();
      weShouldntUseColours =
          parsed["no-colour"].as<bool>() || parsed["no-color"].as<bool>();
    } catch (const cxxopts::OptionParseException &e) {
      std::cerr << e.what() << '\n' << options.help();
      return 1;
    }

    if (!weShouldBeQuiet) {
      Callback errors(
          "LC3AS",
          [&errors](auto &&, auto &&diagnostic) {
            errors.error(
                "{}: {}",
                fmt::format("{0:s}{1:s}{2:s}",
                            Console::Colour(Console::FOREGROUND_COLOUR::RED),
                            "Error", Console::reset),
                diagnostic);
          },
          false, false);

      error_notifications << errors;

      Callback warnings(
          "LC3AS",
          [&warnings, weShouldTreatWarningsAsErrors,
           weShouldntDisplayWarnings](auto &&, auto &&diagnostic) {
            if (weShouldTreatWarningsAsErrors) {
              warnings.error(
                  "{}: {}",
                  fmt::format("{0:s}{1:s}{2:s}",
                              Console::Colour(Console::FOREGROUND_COLOUR::RED),
                              "Error", Console::reset),
                  diagnostic);
            } else if (!weShouldntDisplayWarnings) {
              warnings.warn("{}: {}",
                            fmt::format("{0:s}{1:s}{2:s}",
                                        Console::Colour(
                                            Console::FOREGROUND_COLOUR::YELLOW),
                                        "Warning", Console::reset),
                            diagnostic);
            }
          },
          false, false);

      warning_notifications << warnings;
    }

    int retValue = 0;

    for (auto &file : files) {
      Lexer lexer(File{file}, weShouldTreatWarningsAsErrors);

      lexer.lex();

      if (weShouldPrintAST) {
        fmt::print("{}\n", lexer);
      }

      warning_notifications.notify_all_and_clear();

      if (lexer.isOkay()) {
        Parser parser(std::move(lexer.tokens));

        parser.parse();

        warning_notifications.notify_all_and_clear();

        if (parser.isOkay()) {
          auto file_name = file.substr(0, file.find('.'));
          auto bin_file_name = file_name + ".bin";
          auto hex_file_name = file_name + ".hex";
          auto lst_file_name = file_name + ".lst";
          auto obj_file_name = file_name + ".obj";

          auto bin_file = std::ofstream(bin_file_name);
          auto hex_file = std::ofstream(hex_file_name);
          auto lst_file = std::ofstream(lst_file_name);
          auto obj_file = std::ofstream(obj_file_name, std::ios::binary);

          auto &&tokens = parser.tokens();
          auto &&symbols = parser.symbols();

          int16_t program_counter{0};
          // Assemble the Orig statement simply because it's required as the
          // starting program counter
          (*(tokens.begin()))->assemble(program_counter, 0, std::string());

          program_counter = static_cast<int16_t>(
              ((*(tokens.begin()))->assembled().front().binary()));

          for (auto &&token : tokens) {
            token->assemble(program_counter, 30, std::string());
          }

          for (auto &&token : tokens) {
            if (token->tokenType() == ::Lexer::LABEL) {
              // Labels don't get assembled
              continue;
            }

            auto &&assembled = token->assembled();
#ifndef NDEBUG
            if (assembled.empty()) {
              DEBUG("Assembly not implemented for {}", *token);
            }
#endif
            for (auto &&word : assembled) {
              DEBUG("Assembled: {0:0>16b} {0:0>4X} {1}  for {2}", word.binary(),
                    word.lstStr(), *token);
              bin_file << fmt::format("{:0>16b}\n", word.binary());
              hex_file << fmt::format("{:0>4X}\n", word.binary());
              lst_file << word.lstStr() << '\n';
              obj_file << word.binary();
            }
          }
        } else {
          error_notifications.notify_all_and_clear();
        }
      } else {
        retValue = 1;
        error_notifications.notify_all_and_clear();
      }
    }

    return retValue;
  }

private:
  int argumentCount;
  char **argumentValues;
  cxxopts::Options options;
};

#endif
