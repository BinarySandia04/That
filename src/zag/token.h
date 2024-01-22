#pragma once

#include <string>

namespace Zag {

enum TokenType {
  // One character tokens
  TOKEN_LEFT_PAREN, // (
  TOKEN_RIGHT_PAREN, // )
  TOKEN_LEFT_BRACE, // {
  TOKEN_RIGHT_BRACE, // }
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
  TOKEN_GREATER, // >
  TOKEN_GREATER_EQUAL, // >=
  TOKEN_LESSER, // <
  TOKEN_LESSER_EQUAL, // <=
  TOKEN_AMP, // &
  TOKEN_AMP_AMP, // &&
  TOKEN_PIPE, // |
  TOKEN_PIPE_PIPE, // ||
  TOKEN_DOT_DOT, // ..
  
  TOKEN_ARROW, // =>

  // Literals
  TOKEN_IDENTIFIER, // hola
  TOKEN_STRING, // "gola"
  TOKEN_NUMBER, // 89.3

  // Keywords
  TOKEN_IF, // if
  TOKEN_ELIF, // elif
  TOKEN_ELSE, // else
  TOKEN_FN, // fn
  TOKEN_FOR, // for
  TOKEN_RETURN, // ret
  TOKEN_SUPER, // super
  TOKEN_THIS, // this
  TOKEN_CLASS, // class
  TOKEN_SET, // set

  TOKEN_TRUE, // true
  TOKEN_FALSE, // false
  TOKEN_NONE, // none

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
