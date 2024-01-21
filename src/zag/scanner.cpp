#include <unordered_map>
#include <iostream>

#include "scanner.h"
#include "token.h"
#include "zag.h"

using namespace Zag;

Scanner::Scanner(std::string code) {
  source = code;
  start = 0;
  current = 0;
}

void Scanner::ScanTokens(std::vector<Token> *tokens) {
  this->tokens = tokens;

  while (!AtEnd()) {
    start = current;
    ScanToken();
  }

  tokens->push_back(Token(END_OF_FILE, "", "", current));
}

bool Scanner::AtEnd() { return current >= source.size(); }

void Scanner::ScanToken() {
  char c = source[current];
  Advance();

  switch (c) {
  case '(':
    AddToken(LEFT_PAREN);
    break;
  case ')':
    AddToken(RIGHT_PAREN);
    break;
  case '{':
    AddToken(LEFT_BRACE);
    break;
  case '}':
    AddToken(RIGHT_BRACE);
    break;
  case ',':
    AddToken(COMMA);
    break;
  case '.':
    if (Match('.'))
      AddToken(DOT_DOT);
    else if (!IsDigit(Peek())) {
      AddToken(DOT);
    }
    break;
  case ';':
    AddToken(SEMICOLON);
    break;
  case '|':
    AddToken(Match('|') ? PIPE_PIPE : PIPE);
    break;
  case '!':
    AddToken(Match('=') ? BANG_EQUAL : BANG);
    break;
  case '=':
    AddToken(Match('=') ? EQUAL_EQUAL : EQUAL);
    return;
  case '<':
    AddToken(Match('=') ? LESSER_EQUAL : LESSER);
    break;
  case '>':
    AddToken(Match('=') ? GREATER_EQUAL : GREATER);
    break;
  case '/':
    if (Match('/')) {
      while (Peek() != '\n' && !AtEnd())
        Advance();
    } else if(Match('*')){
      std::cout << "Hola" << std::endl;
      while((Peek() != '*' || PeekNext() != '/') && !AtEnd()){
        Advance();
      }

      // Ignorem el ultim '*/'
      Advance();
      Advance();
    } else {
      AddToken(SLASH);
    }
    break;
  case ' ':
    break;
  case '\r':
    break;
  case '\t':
    break;
  case '\n':
    break;

  case '"':
    GetString();
    break;
  default:
    // Other checks
    if (IsDigit(c)) {
      Number();
    } else if (IsAlpha(c)) {
      Identifier();
    } else {
      std::cout << c << std::endl;
      throw(Error(current, "Unexpected character"));
    }
  }
}

void Scanner::AddToken(TokenType type) { AddToken(type, ""); }

void Scanner::AddToken(TokenType type, std::string content) {
  std::string text = source.substr(start, current - start);
  tokens->push_back(Token(type, text, content, current));
}

char Scanner::Advance() {
  current++;
  return source[current];
}

bool Scanner::Match(char expected) {
  if (AtEnd())
    return false;
  if (source[current] != expected)
    return false;

  current++;
  return true;
}

char Scanner::Peek() {
  if (AtEnd())
    return '\0';
  return source[current];
}

void Scanner::GetString() {
  while (Peek() != '"' && !AtEnd()) {
    Advance();
  }

  if (AtEnd()) {
    throw(Error(current, "String not terminated"));
    return;
  }

  // The " remaining
  Advance();

  AddToken(STRING, source.substr(start + 1, current - start - 2));
}

bool Scanner::IsDigit(char c) { return IsDigitNumber(c) || c == '.'; }

void Scanner::Number() {
  while (IsDigitNumber(Peek()))
    Advance();
  if (Peek() == '.' && IsDigitNumber(PeekNext())) {
    Advance();
    while (IsDigit(Peek()))
      Advance();
  }

  AddToken(NUMBER, source.substr(start, current - start));
}

bool Scanner::IsDigitNumber(char c) { return c >= '0' && c <= '9'; }

char Scanner::PeekNext() {
  if (current + 1 >= source.size())
    return '\0';
  return source[current + 1];
}

bool Scanner::IsAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::IsAlphaNumeric(char c) { return IsAlpha(c) || IsDigitNumber(c); }

void Scanner::Identifier() {
  while (IsAlphaNumeric(Peek()))
    Advance();

  std::string text = source.substr(start, current - start);
  
  TokenType type = keywords[text];
  if(type) type = IDENTIFIER;

  AddToken(type);
}

std::unordered_map<std::string, TokenType> Scanner::keywords {
  {"if", IF},
  {"else", ELSE},
  {"elif", ELIF},
  {"for", FOR},
  {"fn", FN},
  {"this", THIS},
  {"super", SUPER},
  {"none", NONE},
  {"return", RETURN},
  {"class", CLASS},
  {"true", TRUE},
  {"false", FALSE}
};
