#include "token.h"

using namespace Zag;

Token::Token(TokenType type, std::string lexeme, std::string literal,
             int offset) {
  this->type = type;
  this->lexeme = lexeme;
  this->offset = offset;
  this->literal = literal;
}

std::string Token::ToString() { return "TYPE-ID: " + std::to_string(type) + " | LEX: " + lexeme + " | LITERAL: " + literal; }
