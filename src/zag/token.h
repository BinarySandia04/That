#pragma once

#include <string>

namespace Zag {

enum TokenType {
  // One character tokens
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_DOUBLE_DOTS,
  TOKEN_MINUS,
  TOKEN_PLUS,
  TOKEN_SEMICOLON,
  TOKEN_SLASH,
  TOKEN_STAR,

  // One or more character tokens
  TOKEN_BANG,
  TOKEN_BANG_EQUAL,
  TOKEN_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESSER,
  TOKEN_LESSER_EQUAL,
  TOKEN_AMP,
  TOKEN_AMP_AMP,
  TOKEN_PIPE,
  TOKEN_PIPE_PIPE,
  TOKEN_DOT_DOT,

  // Literals
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_NUMBER,

  // Keywords
  TOKEN_IF,
  TOKEN_ELIF,
  TOKEN_ELSE,
  TOKEN_FN,
  TOKEN_FOR,
  TOKEN_RETURN,
  TOKEN_SUPER,
  TOKEN_THIS,
  TOKEN_CLASS,

  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_NONE,

  // End of file
  TOKEN_END_OF_FILE

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
