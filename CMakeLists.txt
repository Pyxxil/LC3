cmake_minimum_required(VERSION 3.8)
project(LC3Rewrite)

set(CMAKE_CXX_STANDARD 20)

set(INCLUDE_FILES
    Debug.hpp
    Notifier.hpp
    Callback.hpp
    Console.hpp
    Diagnostic.hpp
    Algorithm.hpp
    cxxopts.hpp
    Lexer/Lexer.hpp
    Lexer/Token.hpp
    Lexer/Parser.hpp
    Lexer/Line.hpp
    Lexer/File.hpp
    Lexer/Tokens.hpp
    Lexer/Tokenizer.hpp
    Lexer/Tokens/Add.hpp
    Lexer/Tokens/Register.hpp
    Lexer/Tokens/Immediate.hpp
    Lexer/Tokens/String.hpp
    Lexer/Tokens/Character.hpp
    Lexer/Tokens/Add.hpp
    Lexer/Tokens/And.hpp
    Lexer/Tokens/Binary.hpp
    Lexer/Tokens/Blkw.hpp
    Lexer/Tokens/Br.hpp
    Lexer/Tokens/Character.hpp
    Lexer/Tokens/Comment.hpp
    Lexer/Tokens/Decimal.hpp
    Lexer/Tokens/End.hpp
    Lexer/Tokens/Fill.hpp
    Lexer/Tokens/Getc.hpp
    Lexer/Tokens/Halt.hpp
    Lexer/Tokens/Hexadecimal.hpp
    Lexer/Tokens/In.hpp
    Lexer/Tokens/Include.hpp
    Lexer/Tokens/Jmp.hpp
    Lexer/Tokens/Jsr.hpp
    Lexer/Tokens/Jsrr.hpp
    Lexer/Tokens/Label.hpp
    Lexer/Tokens/Ld.hpp
    Lexer/Tokens/Ldi.hpp
    Lexer/Tokens/Ldr.hpp
    Lexer/Tokens/Lea.hpp
    Lexer/Tokens/Lshift.hpp
    Lexer/Tokens/Neg.hpp
    Lexer/Tokens/Not.hpp
    Lexer/Tokens/Octal.hpp
    Lexer/Tokens/Orig.hpp
    Lexer/Tokens/Out.hpp
    Lexer/Tokens/Puts.hpp
    Lexer/Tokens/Putsp.hpp
    Lexer/Tokens/Ret.hpp
    Lexer/Tokens/Rti.hpp
    Lexer/Tokens/Set.hpp
    Lexer/Tokens/St.hpp
    Lexer/Tokens/Sti.hpp
    Lexer/Tokens/Str.hpp
    Lexer/Tokens/Stringz.hpp
    Lexer/Tokens/Trap.hpp
    Lexer/Tokens/Sub.hpp)

set(LIB_SOURCE_FILES Lexer/File.cpp)

set(TEST_SOURCE_FILES Test.cpp)

set(ASSEMBLER_SOURCE_FILES Assembler.hpp Assembler.cpp)

set(DISASSEMBLER_SOURCE_FILES Disassembler.hpp Disassembler.cpp)

set(FORMATTER_SOURCE_FILES Format.hpp Format.cpp)

set(DEBUG_FLAGS ${COMPILE_FLAGS} -g -g3 -pg -pedantic)

if(NOT WIN32)
  # set(DEBUG_FLAGS ${DEBUG_FLAGS} -Wall -Werror -Wextra -Wshadow -Wnon-virtual-
  # dtor -Wconversion -Wno-error=conversion) set(DEBUG_FLAGS ${DEBUG_FLAGS}
  # -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual) set(DEBUG_FLAGS
  # ${DEBUG_FLAGS} -Wsign-conversion -Wno-error=shadow -Wno-error=missing-braces
  # -Wno-error=sign-conversion)
endif()

option(USE_ADDONS "Use Addons when assembling")

set(RELEASE_FLAGS ${COMPILE_FLAGS} -flto -g)

# set(CMAKE_VERBOSE_MAKEFILE True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

include(FetchContent)
fetchcontent_declare(fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt
	GIT_PROGRESS TRUE
	USES_TERMINAL_DOWNLOAD TRUE
)

fetchcontent_getproperties(fmt)
if(NOT fmt_POPULATED)
  fetchcontent_populate(fmt)
  add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif()

add_library(LC3 STATIC ${LIB_SOURCE_FILES} ${INCLUDE_FILES})
include_directories(LC3 ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer)
target_compile_options(LC3 INTERFACE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(LC3 INTERFACE "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")

add_executable(Test ${TEST_SOURCE_FILES})
target_compile_options(Test PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(Test PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
target_link_libraries(Test LC3 fmt)

add_executable(LC3AS ${ASSEMBLER_SOURCE_FILES})
include_directories(LC3AS ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer)
include_directories(LC3AS ${fmt_SOURCE_DIR})
target_compile_options(LC3AS PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(LC3AS PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
target_link_libraries(LC3AS LC3 fmt)

add_executable(LC3DIS ${DISASSEMBLER_SOURCE_FILES})
include_directories(LC3DIS ${fmt_SOURCE_DIR})
target_compile_options(LC3DIS PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(LC3DIS PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
target_link_libraries(LC3DIS LC3 fmt)

add_executable(LC3Format ${FORMATTER_SOURCE_FILES})
target_compile_options(LC3Format PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_compile_options(LC3Format PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
target_compile_definitions(LC3Format PRIVATE KEEP_COMMENTS)
target_link_libraries(LC3Format LC3 fmt::fmt)

if(USE_ADDONS)
  target_compile_definitions(LC3 INTERFACE ADDONS)
  target_compile_definitions(Test PUBLIC ADDONS)
endif(USE_ADDONS)
