#pragma once

#include <string>

namespace Zag {

enum TokenType {
  // One character tokens
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  DOUBLE_DOTS,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // One or more character tokens
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESSER,
  LESSER_EQUAL,
  AMP,
  AMP_AMP,
  PIPE,
  PIPE_PIPE,
  DOT_DOT,

  // Literals
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords
  IF,
  ELIF,
  ELSE,
  FN,
  FOR,
  RETURN,
  SUPER,
  THIS,
  CLASS,

  TRUE,
  FALSE,
  NONE,

  // End of file
  END_OF_FILE

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
