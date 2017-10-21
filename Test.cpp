#include <fstream>
#include <iostream>
#include <memory>

#include "Callback.hpp"
#include "Notifier.hpp"

#include "Lexer/Lexer.hpp"
#include "Lexer/Line.hpp"
#include "Lexer/Token.hpp"
#include "Lexer/Tokens/Add.hpp"
#include "Lexer/Tokens/Immediate.hpp"
#include "Lexer/Tokens/Register.hpp"

int
main(int argc, char** argv)
{
  (void)argc;
  (void)argv;
  using Event = Notification::NOTIFY_EVENT;

  Notification::Callback c(
    "Test", [](auto&& s) { std::cout << s << ": Hello World!\n"; });
  Notification::Notifier<Event::DIAGNOSTIC> d(c);

  Notification::add_notification(d);
  // Notifier's can also be created with anonymous callbacks.
  Notification::add_notification(Notification::Notifier<Event::DIAGNOSTIC>(
    [](auto&& s) { std::cout << s << ": Goodbye World!\n"; }));

  // Callbacks can be added to a specific notifier this way as well.
  // Note that this ends up being roughly the same as just creating a notifier,
  // just without an L-Value to use.
  Notification::add_callback<Event::DIAGNOSTIC>(c);

  Notification::diagnostic_notifications.for_each(
    [](auto&& notification) { notification.notify(); });

  Lexer::Lexer lexer;

  lexer << std::make_shared<Lexer::Token::Add>("Add")
        << std::make_shared<Lexer::Token::Register>("R1")
        << std::make_shared<Lexer::Token::Register>("R2")
        << std::make_shared<Lexer::Token::Immediate>("10")

        << std::make_shared<Lexer::Token::Add>("ADD")
        << std::make_shared<Lexer::Token::Register>("R3")
        << std::make_shared<Lexer::Token::Register>("R1")
        << std::make_shared<Lexer::Token::Register>("R2")

        << Lexer::Line("\tAdd R1 R2 '\n'; Yay for comments!")
        << Lexer::Line("\"Hello\" \"\\\"\" \"")
        << Lexer::Line("'H'  'Hello'  '\n'")
        << Lexer::Line(
             "Jmp R7 JSR  LABEL JSRR R2 NOT R2 , R3 .FILL -x300 AND R1, R2")
        << std::make_shared<Lexer::Token::Immediate>("144");

  std::ifstream f{ "/Users/pyxxil/Sync/Projects/LC3/Examples/Fibonacci.asm" };
  std::string line;
  while (std::getline(f, line)) {
    lexer << Lexer::Line(line);
  }
  std::ifstream z{
    "/Users/pyxxil/Sync/Projects/LC3/Examples/Recursive_Fibonacci.asm"
  };
  while (std::getline(z, line)) {
    lexer << Lexer::Line(line);
  }
  std::ifstream t{ "/Users/pyxxil/Sync/Projects/LC3/Examples/Compare.asm" };
  while (std::getline(t, line)) {
    lexer << Lexer::Line(line);
  }
  std::ifstream e{ "/Users/pyxxil/Sync/Projects/LC3/Examples/input.asm" };
  while (std::getline(e, line)) {
    lexer << Lexer::Line(line);
  }
  std::ifstream a{
    "/Users/pyxxil/Sync/Projects/LC3/Examples/Multi_Word_Addition.asm"
  };
  while (std::getline(a, line)) {
    lexer << Lexer::Line(line);
  }

  lexer.lex();

  for (auto&& t : lexer.tokens) {
    std::cout << "Token: " << t->getToken()
              << ", Type: " << Lexer::TokenTypeString[t->tokenType()] << '\n';
  }

  Notification::diagnostic_notifications.notify_all_and_clear();
  Notification::diagnostic_notifications.for_each(
    [](auto&& notification) { notification.notify(); });

  return 0;
}