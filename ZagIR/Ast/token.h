#pragma once

#include <string>

namespace ZagIR {

enum TokenType {
  // One character tokens
  TOKEN_LEFT_PAREN, // (
  TOKEN_RIGHT_PAREN, // )
  TOKEN_LEFT_BRACE, // {
  TOKEN_RIGHT_BRACE, // }
  TOKEN_LEFT_BRACKET, // [
  TOKEN_RIGHT_BRACKET, // ]
  TOKEN_COMMA,      // ,
  TOKEN_DOT,        // .
  TOKEN_DOUBLE_DOTS, // :
  TOKEN_DOUBLE_DOUBLE_DOTS, // ::
  TOKEN_AT, // @

  TOKEN_MINUS, // -
  TOKEN_PLUS, // +
  TOKEN_SEMICOLON, // ;
  TOKEN_SLASH, // / 
  TOKEN_STAR, // * 
  TOKEN_PERCENTAGE, // %

  TOKEN_MINUS_MINUS, // --
  TOKEN_PLUS_PLUS, // ++
  
  TOKEN_MINUS_EQUAL, // -=
  TOKEN_PLUS_EQUAL, // +=
  TOKEN_SLASH_EQUAL, // /=
  TOKEN_STAR_EQUAL, // *= 
  TOKEN_PERCENTAGE_EQUAL, // %= 

  // One or more character tokens
  TOKEN_BANG, // !
  TOKEN_BANG_EQUAL, // !=
  TOKEN_EQUAL, // =
  TOKEN_EQUAL_EQUAL, // ==
  TOKEN_DOUBLE_DOTS_EQUAL, // :=
  TOKEN_GREATER, // > 
  TOKEN_GREATER_EQUAL, // >=
  TOKEN_LESSER, // <
  TOKEN_LESSER_EQUAL, // <=
  TOKEN_AMP, // &
  TOKEN_DOLLAR, // $
  TOKEN_AMP_AMP, // &&
  TOKEN_PIPE, // |
  TOKEN_PIPE_EQUAL, // |=
  TOKEN_PIPE_PIPE, // ||
  TOKEN_DOT_DOT, // ..
  
  TOKEN_ARROW, // =>

  // Literals
  TOKEN_IDENTIFIER, // hola
  TOKEN_STRING, // "gola"
  TOKEN_NUMBER, // 89.3 
  TOKEN_CONST, // 'Hola

  // Keywords
  TOKEN_IF, // if
  TOKEN_EIF, // eif
  TOKEN_ELS, // els
  TOKEN_FN, // fn
  TOKEN_LUP, // for
  TOKEN_BRK, // brk
  TOKEN_RET, // ret
  TOKEN_SUP, // sup
  TOKEN_DIS, // dis
  TOKEN_KIN, // kin
  TOKEN_GET, // get
  TOKEN_HOC, // hoc

  TOKEN_YEP, // yep
  TOKEN_NOP, // nop
  TOKEN_NIL, // nil

  // End of file
  TOKEN_END_OF_FILE // eof

};

class Token {
public:
  TokenType type;

  std::string lexeme;
  int offset;
  
  std::string literal;

  Token(TokenType type, std::string lexeme, std::string literal, int offset);
  std::string ToString();

private:
};

} // namespace Zag
