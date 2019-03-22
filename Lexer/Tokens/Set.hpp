#ifndef TOKEN_SET_HPP
#define TOKEN_SET_HPP

#include "../Token.hpp"

namespace Lexer {
namespace Token {
#ifdef ADDONS
class Set : public Token {
public:
  explicit Set(const std::string &t, size_t tLine, size_t tColumn,
               const std::string &tFile)
      : Token(std::move(t), tLine, tColumn, tFile,
              Requirements(2, {Match(TokenType::REGISTER),
                               Match(TokenType::IMMEDIATE) |
                                   Match(TokenType::LABEL)})) {}

  Set(const Set &) = default;
  Set(Set &&) = default;

  Set &operator=(const Set &) = default;
  Set &operator=(Set &&) = default;

  TokenType token_type() const final { return SET; }

  void assemble(int16_t &programCounter, size_t width,
                const std::map<std::string, Symbol> &symbols) override {
    const auto &ops = operands();

    const int16_t value = static_cast<Immediate *>(ops[1].get())->value();

    if (value > -16 && value < 15) {
      And a("AND", line(), column(), file());
      a.add_operand(std::make_unique<Register>(
          fmt::format("R{:d}",
                      static_cast<Register *>(ops.front().get())->reg()),
          line(), column(), file()));
      a.add_operand(std::make_unique<Register>(
          fmt::format("R{:d}",
                      static_cast<Register *>(ops.front().get())->reg()),
          line(), column(), file()));
      a.add_operand(
          std::make_unique<Immediate>("#0", line(), column(), file()));
      a.assemble(programCounter, width, symbols);

      Add ad("ADD", line(), column(), file());
      ad.add_operand(std::make_unique<Register>(
          fmt::format("R{:d}",
                      static_cast<Register *>(ops.front().get())->reg()),
          line(), column(), file()));
      ad.add_operand(std::make_unique<Register>(
          fmt::format("R{:d}",
                      static_cast<Register *>(ops.front().get())->reg()),
          line(), column(), file()));
      ad.add_operand(std::make_unique<Decimal>(fmt::format("#{:d}", value),
                                               line(), column(), file()));
      ad.assemble(programCounter, width, symbols);

      for (auto &&as : a.assembled()) {
        as_assembled.emplace_back(as);
      }

      for (auto &&as : ad.assembled()) {
        as_assembled.emplace_back(as);
      }
    } else {
      Br b("BR", true, true, true, line(), column(), file());
      b.add_operand(std::make_unique<Decimal>("#1", line(), column(), file()));
      b.assemble(programCounter, width, symbols);

      Fill f(".FILL", line(), column(), file());
      f.add_operand(std::make_unique<Decimal>(fmt::format("#{:d}", value),
                                              line(), column(), file()));
      f.assemble(programCounter, width, symbols);

      Ld l("LD", line(), column(), file());
      l.add_operand(std::make_unique<Register>(
          fmt::format("R{:d}",
                      static_cast<Register *>(ops.front().get())->reg()),
          line(), column(), file()));
      l.add_operand(std::make_unique<Decimal>("#-2", line(), column(), file()));
      l.assemble(programCounter, width, symbols);

      for (auto &&as : b.assembled()) {
        as_assembled.emplace_back(as);
      }

      for (auto &&as : f.assembled()) {
        as_assembled.emplace_back(as);
      }

      for (auto &&as : l.assembled()) {
        as_assembled.emplace_back(as);
      }
    }
  }

  word memory_required() const override {
    const auto immediate_value =
        static_cast<Immediate *>(operands().back().get())->value();
    return static_cast<word>(
        (immediate_value > 15 || immediate_value < -16) ? 3 : 2);
  }
};
#endif
} // namespace Token
} // namespace Lexer

#endif