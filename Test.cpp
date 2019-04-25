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

int main(int /* unused */, char ** /* unused */) {
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

  Notification::diagnostic_notifications.each(
      [](auto &&d) { fmt::print("{}", d); });

  Lexer::Lexer lexer;

  lexer << "\tAdd R1 R2 '\n'; Yay for comments!"sv
        << ".STRINGZ \"Hello\" \"\\\"\" \"\""sv
        << ".FILL 'H'  .sTRINGZ \"Hello\"  .FILL '\n'"sv
        << "Jmp R7 JSR  LABEL JSRR R2 NOT R2 , R3 TEST:.FILL -x300 AND R1, R2"sv
        << ".STRINGZ \"Unterminated string\""sv;

  lexer.lex();

  Notification::warning_notifications.notify_all(true);

  if (lexer.is_okay()) {
    fmt::print("{}", lexer);
  } else {
    Notification::error_notifications.notify_all(true);
  }

  lexer.clear();

  for (auto &&file : {"Test.asm"}) {
    Lexer::Lexer lexer(Lexer::File{file});

    lexer.lex();

    Notification::warning_notifications.notify_each();

    if (lexer.is_okay()) {
      fmt::print("{}", lexer);
    } else {
      Notification::error_notifications.notify_each();
    }
  }

  return 0;
}