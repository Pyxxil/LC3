#include <fstream>
#include <iostream>
#include <memory>

#include "Callback.hpp"
#include "Notifier.hpp"

#include "Lexer/Lexer.hpp"
#include "Lexer/Line.hpp"
#include "Lexer/Token.hpp"
#include "Lexer/Tokens.hpp"

#if __APPLE__
#define TEST_FILE_PATH "/Users/pyxxil/Sync/Projects/LC3"
#elif __linux__
#define TEST_FILE_PATH "/home/pyxxil/Sync/Projects/LC3"
#endif

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

  lexer << new Lexer::Token::Add("Add") << new Lexer::Token::Register("R1")
        << new Lexer::Token::Register("R2") << new Lexer::Token::Immediate("10")

        << new Lexer::Token::Add("ADD") << new Lexer::Token::Register("R3")
        << new Lexer::Token::Register("R1") << new Lexer::Token::Register("R2")

        << "\tAdd R1 R2 '\n'; Yay for comments!"
        << "\"Hello\" \"\\\"\" \""
        << "'H'  'Hello'  '\n'"
        << "Jmp R7 JSR  LABEL JSRR R2 NOT R2 ,, R3 TEST:.FILL, -x300 AND R1, R2"
        << new Lexer::Token::Immediate("144") << "\"Unterminated string";

  std::string line;
  std::ifstream test{ TEST_FILE_PATH "/LC3/Examples/Test.asm" };
  while (std::getline(test, line)) {
    lexer << line;
  }

  std::ifstream f{ TEST_FILE_PATH "/Examples/Fibonacci.asm" };
  while (std::getline(f, line)) {
    lexer << line;
  }
  std::ifstream z{ TEST_FILE_PATH "/Examples/Recursive_Fibonacci.asm" };
  while (std::getline(z, line)) {
    lexer << line;
  }
  std::ifstream v{ TEST_FILE_PATH "/Examples/Compare.asm" };
  while (std::getline(v, line)) {
    lexer << line;
  }
  std::ifstream e{ TEST_FILE_PATH "/Examples/input.asm" };
  while (std::getline(e, line)) {
    lexer << line;
  }
  std::ifstream a{ TEST_FILE_PATH "/Examples/Multi_Word_Addition.asm" };
  while (std::getline(a, line)) {
    lexer << line;
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