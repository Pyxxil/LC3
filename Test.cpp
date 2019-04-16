#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

#include "Callback.hpp"
#include "Notifier.hpp"

#include "File.hpp"
#include "Lexer/Lexer.hpp"
#include "Lexer/Line.hpp"
#include "Lexer/Token.hpp"
#include "Lexer/Tokens.hpp"

#include "fmt/printf.h"

#if __APPLE__
#define TEST_FILE_PATH "/Users/pyxxil/Sync/Projects/LC3/Examples"
#elif __linux__
#define TEST_FILE_PATH "/home/pyxxil/Sync/Projects/LC3/Examples"
#endif

auto main(int, char **) -> int {
  using namespace std::literals; // For string_view literals
  using namespace Lexer::Token;

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
        warnings.warn(
            "{}: {}",
            fmt::format("{0:s}{1:s}{2:s}",
                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW),
                        "Warning", Console::reset),
            diagnostic);
      },
      false, false);
  Notification::warning_notifications << warnings;

  Notification::Callback test("Test", [](auto &&, auto &&diagnostic) {
    fmt::print("{{ \"line\": \"{}\", \"column\": \"{}\", \"message\": \"{}\", "
               "\"file\": \"{}\" }}\n",
               diagnostic.line(), diagnostic.column(), diagnostic.message(),
               diagnostic.file());
  });
  Notification::warning_notifications << test;
  Notification::error_notifications << test;

  // Notification::diagnostic_notifications.for_each(
  //     [](auto &&diagnostic) { std::cout << diagnostic << '\n'; });

  Lexer::Lexer lexer;

  lexer
      << "\tAdd R1 R2 '\n'; Yay for comments!"sv
      << "\"Hello\" \"\\\"\" \""sv
      << "'H'  'Hello'  '\n'"sv
      << "Jmp R7 JSR  LABEL JSRR R2 NOT R2 ,, R3 TEST:.FILL, -x300 AND R1, R2"sv
      << "\"Unterminated string"sv;

  lexer.lex();

  Notification::warning_notifications.notify_all(true);

  if (lexer.is_okay()) {
    std::cout << lexer;
  } else {
    Notification::error_notifications.notify_all(true);
  }

  lexer.clear();

  for (auto &&file : {
           "Test.asm", // TEST_FILE_PATH "/Fibonacci.asm",
                       // TEST_FILE_PATH "/Recursive_Fibonacci.asm",
           // TEST_FILE_PATH "/Compare.asm", TEST_FILE_PATH "/input.asm",
           // TEST_FILE_PATH "/Multi_Word_Addition.asm"
           // TEST_FILE_PATH "/Features.asm"
       }) {
    Lexer::Lexer lexer(Lexer::File{file});

    lexer.lex();

    Notification::warning_notifications.notify_each();

    if (lexer.is_okay()) {
      fmt::print("{}\n", lexer);
    } else {
      Notification::error_notifications.notify_each();
    }
  }

  return 0;
}