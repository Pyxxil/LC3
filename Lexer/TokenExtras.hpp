#ifndef TOKENEXTRAS_HPP
#define TOKENEXTRAS_HPP

using word = int;
constexpr word operator""_word(unsigned long long val) {
  return static_cast<word>(val);
}

constexpr word operator""_words(unsigned long long val) {
  return static_cast<word>(val);
}

enum TokenType {
  NONE = 0,
  ADD,
  AND,
  BR,
  NOT,
  JMP,
  JSR,
  JSRR,
  LD,
  LEA,
  LDI,
  LDR,
  ST,
  STI,
  STR,
  TRAP,
  HALT,
  PUTS,
  PUTSP,
  PUTC,
  GETC,
  OUT,
  IN,
  RTI,
  RET,
  REGISTER,
  IMMEDIATE,
  STRING,
  LABEL,
  STRINGZ,
  FILL,
  BLKW,
  ORIG,
  END,
#ifdef ADDONS
  INCLUDE,
  LSHIFT,
  SET,
  NEG,
  SUB,
#endif
  JMPT,
#ifdef KEEP_COMMENTS
  COMMENT,
#endif
};

namespace {

const char *TOKEN_TYPE_STRING[] = {
    "INVALID",
    "Instruction ADD",
    "Instruction AND",
    "Instruction BR",
    "Instruction NOT",
    "Instruction JMP",
    "Instruction JSR",
    "Instruction JSRR",
    "Instruction LD",
    "Instruction LEA",
    "Instruction LDI",
    "Instruction LDR",
    "Instruction ST",
    "Instruction STI",
    "Instruction STR",
    "Instruction TRAP",
    "Trap HALT",
    "Trap PUTS",
    "Trap PUTSP",
    "Trap PUTC",
    "Trap GETC",
    "Trap OUT",
    "Trap IN",
    "Instruction RTI",
    "Pseudo Op RET",
    "REGISTER",
    "IMMEDIATE",
    "STRING",
    "LABEL",
    "Directive .STRINGZ",
    "Directive .FILL",
    "Directive .BLKW",
    "Directive .ORIG",
    "Directive .END",
#ifdef ADDONS
    "Directive .INCLUDE",
    "LSHIFT",
    "Directive .SET",
    "Directive .NEG",
    "Directive .SUB",
#endif
    "Instruction JMPT",
#ifdef KEEP_COMMENTS
    "Comment",
#endif
};

} // namespace

template <typename OStream>
inline OStream &operator<<(OStream &os, TokenType type) {
  return os << TOKEN_TYPE_STRING[type];
}

#endif
