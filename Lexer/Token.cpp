#include "Token.hpp"

#include "Requirements.hpp"

namespace Lexer::Token {

Token::Token(std::string t, size_t t_line, size_t t_column, std::string t_file,
             Requirements r)
    : token(std::move(t)), at_line(t_line), at_column(t_column),
      m_file(std::move(t_file)), m_requirements(std::move(r)) {}

void Token::compile_ast() const {
  ast = fmt::format("  {} ( '{}' ) [ File: {}, Line: {}, Column: {} ] {{",
                    token_type(), get_token(), file(), line(), column());
  for (const auto &operand : operands()) {
    if (operand->token_type() != STRING
#ifdef ADDONS
        && '\'' != operand->get_token().front()
#endif
    ) {
      ast += fmt::format("\n    {} ( '{}' ) [ File: {}, Line: {}, Column: {} ]",
                         operand->token_type(), *operand, operand->file(),
                         operand->line(), operand->column());
    } else {
      ast += fmt::format("\n    {} ( {} ) [ File: {}, Line: {}, Column: {} ]",
                         operand->token_type(), *operand, operand->file(),
                         operand->line(), operand->column());
    }
  }
  ast += fmt::format("{0} }}\n", !operands().empty() ? "\n " : std::string{});
  ast_compiled = true;
}

const std::string &Token::AST() const {
  if (!ast_compiled) {
    compile_ast();
  }

  return ast;
}

const std::string &Token::get_token() const { return token; }

TokenType Token::token_type() const { return NONE; }

void Token::add_operand(std::unique_ptr<Token> operand) {
  ast_compiled = false;
  m_operands.emplace_back(std::move(operand));
}

void Token::set_assembled(AssembledToken assembled) {
  as_assembled = {std::move(assembled)};
}

const std::vector<AssembledToken> &Token::assembled() const {
  return as_assembled;
}

} // namespace Lexer::Token
