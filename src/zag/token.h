#pragma once

#include <string>

namespace Zag {

enum TokenType {
  // One character tokens
  TOKEN_LEFT_PAREN, // (    0
  TOKEN_RIGHT_PAREN, // )   1
  TOKEN_LEFT_BRACE, // {    2
  TOKEN_RIGHT_BRACE, // }   3
  TOKEN_COMMA,      // ,    4
  TOKEN_DOT,        // . 5
  TOKEN_DOUBLE_DOTS, // : 6
  TOKEN_DOUBLE_DOUBLE_DOTS, // :: 7
  TOKEN_AT, // @ 8

  TOKEN_MINUS, // - 9
  TOKEN_PLUS, // + 10
  TOKEN_SEMICOLON, // ; 11
  TOKEN_SLASH, // / 12
  TOKEN_STAR, // * 13
  TOKEN_PERCENTAGE, // % 14

  TOKEN_MINUS_MINUS, // -- 15
  TOKEN_PLUS_PLUS, // ++ 16
  
  TOKEN_MINUS_EQUAL, // -= 17
  TOKEN_PLUS_EQUAL, // += 18
  TOKEN_SLASH_EQUAL, // /= 19
  TOKEN_STAR_EQUAL, // *= 20
  TOKEN_PERCENTAGE_EQUAL, // %= 21

  // One or more character tokens
  TOKEN_BANG, // ! 22
  TOKEN_BANG_EQUAL, // != 23
  TOKEN_EQUAL, // = 24
  TOKEN_EQUAL_EQUAL, // == 25
  TOKEN_GREATER, // > 26
  TOKEN_GREATER_EQUAL, // >= 27
  TOKEN_LESSER, // < 28
  TOKEN_LESSER_EQUAL, // <= 29
  TOKEN_AMP, // & 30
  TOKEN_AMP_AMP, // && 31
  TOKEN_PIPE, // | 32
  TOKEN_PIPE_PIPE, // || 33
  TOKEN_DOT_DOT, // .. 34
  
  TOKEN_ARROW, // => 35

  // Literals
  TOKEN_IDENTIFIER, // hola 36
  TOKEN_STRING, // "gola" 37
  TOKEN_NUMBER, // 89.3 38

  // Keywords
  TOKEN_IF, // if 39
  TOKEN_ELIF, // elif 40
  TOKEN_ELSE, // else 41
  TOKEN_FN, // fn 42
  TOKEN_FOR, // for 43
  TOKEN_RETURN, // ret 44
  TOKEN_SUPER, // super 45
  TOKEN_THIS, // this 46
  TOKEN_CLASS, // class 47
  TOKEN_SET, // set 48

  TOKEN_TRUE, // true 49
  TOKEN_FALSE, // false 50
  TOKEN_NONE, // none 51

  // End of file
  TOKEN_END_OF_FILE // eof 52

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
