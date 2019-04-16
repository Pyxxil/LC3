#ifndef FORMATTER_HPP

#include <fstream>
#include <iostream>

#include "cxxopts.hpp"
#include "fmt/printf.h"

#include "Algorithm.hpp"
#include "Lexer/Lexer.hpp"

namespace Formatter {
class Formatter {
public:
  Formatter(int arg_count, char **arg_vals)
      : argument_count(arg_count), argument_values(arg_vals),
        options("LC3Format", "A formatter for LC3 Assembly files") {
    parse_options();
  }

  void parse_options() {
    options.positional_help("<files to format>");
    options.add_options()("h,help", "Print this help message")(
        "a,align-comments", "Align all comments")(
        "w,width", "The maximum width of a line",
        cxxopts::value<unsigned int>()->default_value("80"))(
        "files", "The files to format",
        cxxopts::value<std::vector<std::string>>())("no-warn",
                                                    "Don't display warnings")(
        "q,quiet", "Lessen the output to virtually nothing")(
        "i,inplace",
        "Format the file in place, as opposed to creating <file>.formatted")(
        "s,spaces", "Use spaces, not commas, to seperate operands");
    // --case={title, upper, lower}, sets all things to specified case
    // --register-case={title, upper, lower}, sets registers to specified case
    // --immediate-case={title, upper, lower}, sets hex/bin immediate values to
    //   specified case (title is just an alias to upper here)
    // --instruction-case={title, upper, lower}, sets instruction to specified
    //   case
    // --immediate={hex, octal, binary, decimal}, sets all immediate values to
    //   be specified base
    // --indent-width=N, specify the width of an indent
    // --tabs, indent with tabs (--indent-width will be set to 1 by default for
    //   tabs, and 4 otherwise)
    // --no-terminate-labels, don't terminate labels with a ':'
    // --append-prefix, append a '#' if not present on a decimal immediate, '0'
    //   on hexadecimal/binary immediate if not present
    // --remove-prefix, remove a prefix on an immediate if present ('#', or '0'
    //   as above)
    // --fill-to-blkw, attempt to merge multiple of the same .FILL's into .BLKW?
    // --same-line-label, Have instructions on the same line as a label
    // --no-nzp, Turn 'BRnzp' into 'BR'
  }

  int format() {
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

      const bool dont_display_warnings = parsed["no-warn"].as<bool>();
      const bool use_spaces = parsed["spaces"].as<bool>();
      const bool align_comments = parsed["align-comments"].as<bool>();

      if (!parsed["quiet"].as<bool>()) {
        Notification::Callback errors(
            "Test",
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

        Notification::Callback warnings(
            "Test",
            [&warnings](auto &&, auto &&diagnostic) {
              warnings.warn("{}: {}",
                            fmt::format("{0:s}{1:s}{2:s}",
                                        Console::Colour(
                                            Console::FOREGROUND_COLOUR::YELLOW),
                                        "Warning", Console::reset),
                            diagnostic);
            },
            false, false);
        Notification::warning_notifications << warnings;
      }

      for (auto &&file : parsed["files"].as<std::vector<std::string>>()) {
        Lexer::Lexer lexer(Lexer::File{file});
        lexer.lex();

        Notification::warning_notifications.notify_each();

        if (!lexer.is_okay()) {
          Notification::error_notifications.notify_each();
          continue;
        }

        fmt::print("{}\n", lexer);
        std::ofstream formatFile(file + ".formatted");
        auto &&tokens = lexer.tokens;

        for (size_t index = 0; index < tokens.size(); ++index) {
          const auto &token = tokens[index];
#ifdef KEEP_COMMENTS
          if (token->token_type() == TokenType::COMMENT) {
            formatFile << "; " << token->get_token() << '\n';
            continue;
          }
#endif

          std::string line;

          if (token->token_type() == TokenType::LABEL) {
            line.push_back(':');
            line = fmt::format("{}:", token->get_token());
          } else if (token->token_type() == TokenType::ORIG ||
                     token->token_type() == TokenType::END) {
            line = fmt::format("\n{}", token->get_token());
          } else {
            line = fmt::format("    {}", token->get_token());
          }

          if (use_spaces) {
            for (const auto &operand : token->operands()) {
              line += fmt::format(" {}", operand->get_token());
            }
          } else {
            Algorithm::enumerate(
                token->operands().cbegin(), token->operands().cend(),
                [&line](auto &&operand, auto idx) {
                  if (idx > 0) {
                    line += fmt::format(", {}", operand->get_token());
                  } else {
                    line += fmt::format(" {}", operand->get_token());
                  }
                });
          }

#ifdef KEEP_COMMENTS
          if (index + 1 < tokens.size() &&
              tokens[index + 1]->token_type() == TokenType::COMMENT) {
            if (align_comments && line.length() < 40) {
              line += std::string(40 - line.length(), ' ');
            }
            const std::string &tokenString = tokens[++index]->get_token();
            if (tokenString.empty()) {
              line.push_back(';');
            } else {
              line += fmt::format(" ; {}", tokenString);
            }
          }
#endif

          if (token->token_type() == TokenType::ORIG ||
              token->token_type() == TokenType::END) {
            line.push_back('\n');
          }

          formatFile << line << '\n';
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
  cxxopts::Options options;
};
} // namespace Formatter

#endif