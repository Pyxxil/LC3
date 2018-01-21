#include <fstream>
#include <iostream>
#include <memory>

#include "Callback.hpp"
#include "Notifier.hpp"

#include "File.hpp"
#include "Lexer/Lexer.hpp"
#include "Lexer/Line.hpp"
#include "Lexer/Token.hpp"
#include "Lexer/Tokens.hpp"

#include "spdlog/spdlog.h"

#include <string_view>

#if __APPLE__
#define TEST_FILE_PATH "/Users/pyxxil/Sync/Projects/LC3/Examples"
#elif __linux__
#define TEST_FILE_PATH "/home/pyxxil/Sync/Projects/LC3/Examples"
#endif

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  using namespace std::literals; // For string_view literals
  using namespace Lexer::Token;

  spdlog::set_pattern("%v");

  Notification::Callback errors("Test",
                                [&errors](auto &&, auto &&diagnostic) {
                                  errors.error("Error: {}", diagnostic);
                                },
                                false, false);
  Notification::error_notifications << errors;

  Notification::Callback warnings("Test",
                                  [&warnings](auto &&, auto &&diagnostic) {
                                    warnings.warn("Warning: {}", diagnostic);
                                  },
                                  false, false);
  Notification::warning_notifications << warnings;

  Notification::diagnostic_notifications.for_each(
      [](auto &&diagnostic) { std::cout << diagnostic << '\n'; });

  // Lexer::Lexer lexer;

  // lexer
  //     << Add("Add") << Register("R1") << Register("R2") << Immediate("10")

  //     << Add("ADD") << Register("R3") << Register("R1") << Register("R2")

  //     << "\tAdd R1 R2 '\n'; Yay for comments!"sv
  //     << "\"Hello\" \"\\\"\" \""sv
  //     << "'H'  'Hello'  '\n'"sv
  //     << "Jmp R7 JSR  LABEL JSRR R2 NOT R2 ,, R3 TEST:.FILL, -x300 AND R1, R2
  //     "sv
  //     << Immediate("144") << "\"Unterminated string"sv;

  auto ast_console = spdlog::stdout_color_mt("ast_console");
  ast_console->set_level(spdlog::level::info);
  ast_console->set_pattern("%v");

  // lexer.lex();

  // Notification::warning_notifications.notify_all(true);

  // if (lexer.okay()) {
  //   std::cout << lexer;
  // } else {
  //   Notification::error_notifications.notify_all(true);
  // }

  // lexer.clear();

  for (auto &&file :
       {TEST_FILE_PATH "/Test.asm", TEST_FILE_PATH "/Fibonacci.asm",
        TEST_FILE_PATH "/Recursive_Fibonacci.asm",
        TEST_FILE_PATH "/Compare.asm", TEST_FILE_PATH "/input.asm",
        TEST_FILE_PATH "/Multi_Word_Addition.asm"}) {
    Lexer::Lexer lexer(Lexer::File{file});

    lexer.lex();

    Notification::warning_notifications.notify_for_each();

    if (lexer.okay()) {
      ast_console->info("{}\n", lexer);
    } else {
      Notification::error_notifications.notify_for_each();
    }
  }

  return 0;
}