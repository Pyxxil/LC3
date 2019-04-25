#include "Assembler.hpp"

#include <fstream>

#include "Console.hpp"
#include "Lexer/Lexer.hpp"
#include "Lexer/Parser.hpp"

#include "fmt/printf.h"

auto main(int argc, char **argv) -> int {
  Assembler assembler(argc, argv);
  return assembler.assemble();
}

Assembler::Assembler(int argc, char **argv)
    : argument_count(argc), argument_values(argv),
      options("LC3AS", "An assembler for the LC3 Assembly language") {
  parse_options();
}

void Assembler::parse_options() {
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

int Assembler::assemble() {
  using Lexer::File;
  using Lexer::Lexer;
  using Parser::Parser;
  using namespace Notification;

  bool we_should_print_ast;
  bool we_should_treat_warnings_as_errors;
  bool we_should_show_warnings;
  bool we_should_be_quiet;
  bool we_should_not_use_colours;

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

    we_should_print_ast = parsed["print-ast"].as<bool>();
    we_should_treat_warnings_as_errors = parsed["error"].as<bool>();
    we_should_show_warnings = !parsed["no-warn"].as<bool>();
    we_should_be_quiet = parsed["quiet"].as<bool>();
    we_should_not_use_colours =
        parsed["no-colour"].as<bool>() || parsed["no-color"].as<bool>();
  } catch (const cxxopts::OptionParseException &e) {
    std::cerr << e.what() << '\n' << options.help();
    return 1;
  }

  if (!we_should_be_quiet) {
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
        [&warnings, we_should_treat_warnings_as_errors](auto &&,
                                                        auto &&diagnostic) {
          if (we_should_treat_warnings_as_errors) {
            warnings.error(
                "{}: {}",
                fmt::format("{0:s}{1:s}{2:s}",
                            Console::Colour(Console::FOREGROUND_COLOUR::RED),
                            "Error", Console::reset),
                diagnostic);
          } else {
            warnings.warn(
                "{}: {}",
                fmt::format("{0:s}{1:s}{2:s}",
                            Console::Colour(Console::FOREGROUND_COLOUR::YELLOW),
                            "Warning", Console::reset),
                diagnostic);
          }
        },
        false, false);

    if (we_should_show_warnings) {
      warning_notifications << warnings;
    }
  }

  int retValue = 0;

  for (auto &&file : files) {
    int temp_ret_value = 0;
    Lexer lexer(File{file}, we_should_treat_warnings_as_errors);

    lexer.lex();

    if (we_should_print_ast) {
      fmt::print("{}\n", lexer);
    }

    warning_notifications.notify_all_and_clear();

    if (lexer.is_okay()) {
      Parser parser(std::move(lexer.tokens));

      parser.parse();

      warning_notifications.notify_all_and_clear();

      if (parser.is_okay()) {
        const auto file_name = file.substr(0, file.rfind('.'));
        const auto bin_file_name = file_name + ".bin";
        const auto hex_file_name = file_name + ".hex";
        const auto lst_file_name = file_name + ".lst";
        const auto obj_file_name = file_name + ".obj";
        const auto sym_file_name = file_name + ".sym";

        auto bin_file = std::ofstream(bin_file_name);
        auto hex_file = std::ofstream(hex_file_name);
        auto lst_file = std::ofstream(lst_file_name);
        auto obj_file = std::ofstream(obj_file_name, std::ofstream::binary);
        auto sym_file = std::ofstream(sym_file_name);

        auto &&tokens = parser.tokens();
        auto &&symbols = parser.symbols();

        std::vector<::Lexer::Token::AssembledToken> assembled{};
        assembled.reserve(tokens.size());

        uint16_t program_counter{0};
        // Assemble the Orig statement simply because it's required as the
        // starting program counter
        auto &&origin = *tokens.begin();
        origin->assemble(program_counter, 0, symbols, std::string{});

        auto &&[high, low] = origin->assembled().front().binary();
        program_counter = static_cast<int16_t>(high) << 8 | low;

        auto symbol_length = parser.symbol_length();

        fmt::print(sym_file,
                   "// Symbol table\n"
                   "// Scope Level 0:\n"
                   "//\t{}{} Page Address\n"
                   "//\t{} ------------\n",
                   "Symbol Name", std::string(symbol_length - 11, ' '),
                   std::string(symbol_length, '-'));

        for (const auto &[addr, symbol] : symbols) {
          fmt::print(sym_file, "//\t{}{} {:04X}\n", symbol.name(),
                     std::string(symbol_length - symbol.name().length(), ' '),
                     symbol.address());
        }

        for (auto &token : tokens) {
          const auto sym =
              std::find_if(symbols.begin(), symbols.end(),
                           [program_counter](const auto &sym) {
                             return sym.second.address() == program_counter;
                           });
          token->assemble(program_counter, symbol_length, symbols,
                          sym == symbols.end() ? "" : sym->second.name());
        }

        for (auto &&token : tokens) {
          if (token->token_type() == TokenType::LABEL) {
            // Labels don't get assembled
            continue;
          } else if (token->token_type() == TokenType::END) {
            // Nor does the .END
            // Another side effect is that we shouldn't process anything after
            // the .END, so just finish here
            break;
          }

          auto &&t_assembled = token->assembled();
          if (t_assembled.size() != token->memory_required()) {
            temp_ret_value = 1;
          } else {
            assembled.insert(std::end(assembled),
                             std::make_move_iterator(std::begin(t_assembled)),
                             std::make_move_iterator(std::end(t_assembled)));
          }
        }

        if (temp_ret_value == 0) {
          for (auto &&word : assembled) {
            auto &&[high, low] = word.binary();
            const auto bin = static_cast<uint16_t>(high) << 8 | low;
            fmt::print(bin_file, "{:0>16b}\n", bin);
            fmt::print(hex_file, "{:0>4X}\n", bin);
            fmt::print(lst_file, "{}\n", word.lst_str());
            obj_file.put(high).put(low);
          }
        }
      } else {
        temp_ret_value = 1;
      }
    } else {
      temp_ret_value = 1;
    }

    if (temp_ret_value != 0) {
      error_notifications.notify_all_and_clear();
    }

    retValue += temp_ret_value;
  }

  return retValue;
}
