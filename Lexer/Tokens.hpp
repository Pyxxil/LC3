#ifndef TOKENS_HPP
#define TOKENS_HPP

#include "Tokens/Add.hpp"
#include "Tokens/And.hpp"
#include "Tokens/Binary.hpp"
#include "Tokens/Blkw.hpp"
#include "Tokens/Br.hpp"
#include "Tokens/Decimal.hpp"
#include "Tokens/End.hpp"
#include "Tokens/Fill.hpp"
#include "Tokens/Getc.hpp"
#include "Tokens/Halt.hpp"
#include "Tokens/Hexadecimal.hpp"
#include "Tokens/In.hpp"
#include "Tokens/Jmp.hpp"
#include "Tokens/Jsr.hpp"
#include "Tokens/Jsrr.hpp"
#include "Tokens/Label.hpp"
#include "Tokens/Ld.hpp"
#include "Tokens/Ldi.hpp"
#include "Tokens/Ldr.hpp"
#include "Tokens/Lea.hpp"
#include "Tokens/Not.hpp"
#include "Tokens/Octal.hpp"
#include "Tokens/Orig.hpp"
#include "Tokens/Out.hpp"
#include "Tokens/Puts.hpp"
#include "Tokens/Putsp.hpp"
#include "Tokens/Register.hpp"
#include "Tokens/Ret.hpp"
#include "Tokens/Rti.hpp"
#include "Tokens/Set.hpp"
#include "Tokens/St.hpp"
#include "Tokens/Sti.hpp"
#include "Tokens/Str.hpp"
#include "Tokens/String.hpp"
#include "Tokens/Stringz.hpp"
#include "Tokens/Trap.hpp"

#ifdef ADDONS
#include "Tokens/Character.hpp"
#include "Tokens/Include.hpp"
#include "Tokens/Jmpt.hpp"
#include "Tokens/Lshift.hpp"
#include "Tokens/Neg.hpp"
#include "Tokens/Sub.hpp"
#endif

#ifdef KEEP_COMMENTS
#include "Tokens/Comment.hpp"
#endif

#endif