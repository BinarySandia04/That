#include "token.h"

using namespace ZagIR;

Token::Token(TokenType type, std::string lexeme, std::string literal,
             int offset, bool separator) {
  this->type = type;
  this->lexeme = lexeme;
  this->offset = offset;
  this->literal = literal;
  this->separator = separator;
}

std::string Token::ToString() { return "TYPE-ID: " + std::to_string(type) + " | LEX: " + lexeme + " | LITERAL: " + literal; }
