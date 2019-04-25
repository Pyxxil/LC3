#ifndef TOKENEXTRAS_HPP
#define TOKENEXTRAS_HPP

#include <cstdint>

using word = int;
constexpr word operator""_word(unsigned long long val) {
  return static_cast<word>(val);
}

constexpr word operator""_words(unsigned long long val) {
  return static_cast<word>(val);
}

constexpr int IMM5_TOGGLE = 0x20;

template <int shift> constexpr int16_t sign_extend(int16_t val) {
  return static_cast<int16_t>(val << shift) >> shift;
}

template <int bits> constexpr int16_t mask(uint16_t val) {
  return val & ((1 << bits) - 1);
}

enum Opcode : int {
  OP_BR = 0x0000,
  OP_ADD = 0x1000,
  OP_LD = 0x2000,
  OP_ST = 0x3000,
  OP_JSR = 0x4000,
  OP_AND = 0x5000,
  OP_LDR = 0x6000,
  OP_STR = 0x7000,
  OP_NOT = 0x9000,
  OP_LDI = 0xA000,
  OP_STI = 0xB000,
  OP_JMP = 0xC000,
  OP_JMPT = 0xC001,
  OP_LEA = 0xE000,
  OP_TRAP = 0xF000,
};

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
