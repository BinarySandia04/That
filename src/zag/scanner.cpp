#include <iostream>
#include <unordered_map>

#include "error.h"
#include "scanner.h"
#include "token.h"

using namespace Zag;

Scanner::Scanner(std::string code, std::string fileName) {
  source = code;
  
  this->panic = false;
  this->fileName = fileName;

  start = 0;
  current = 0;

  line = 1;
  column = 0;
}

bool Scanner::ScanTokens(std::vector<Token> *tokens, Error *error) {
  this->tokens = tokens;

  while (!AtEnd() && !panic) {
    start = current;
    ScanToken();
  }

  *error = this->error;
  tokens->push_back(Token(TOKEN_END_OF_FILE, "", "", current));

  return !panic; 
}

void Scanner::Panic(Error err){
  panic = true;
  error = err;
}

bool Scanner::AtEnd() { return current >= source.size(); }

void Scanner::ScanToken() {
  char c = source[current];
  Advance();

  switch (c) {
  case '(':
    AddToken(TOKEN_LEFT_PAREN);
    break;
  case ')':
    AddToken(TOKEN_RIGHT_PAREN);
    break;
  case '{':
    AddToken(TOKEN_LEFT_BRACE);
    break;
  case '}':
    AddToken(TOKEN_RIGHT_BRACE);
    break;
  case ',':
    AddToken(TOKEN_COMMA);
    break;
  case '.':
    if (Match('.'))
      AddToken(TOKEN_DOT_DOT);
    else if (!IsDigit(Peek()))
      AddToken(TOKEN_DOT);
    break;
  case ';':
    AddToken(TOKEN_SEMICOLON);
    break;
  case '&':
    AddToken(Match('&') ? TOKEN_AMP_AMP : TOKEN_AMP);
    break;
  case '+':
    if (Match('='))
      AddToken(TOKEN_PLUS_EQUAL);
    else if (Match('+'))
      AddToken(TOKEN_PLUS_PLUS);
    else
      AddToken(TOKEN_PLUS);
    break;
  case '-':
    if (Match('='))
      AddToken(TOKEN_MINUS_EQUAL);
    else if (Match('-'))
      AddToken(TOKEN_MINUS_MINUS);
    else
      AddToken(TOKEN_MINUS);
    break;
  case '*':
    if (Match('='))
      AddToken(TOKEN_STAR_EQUAL);
    else
      AddToken(TOKEN_STAR);
    break;
  case '|':
    AddToken(Match('|') ? TOKEN_PIPE_PIPE : TOKEN_PIPE);
    break;
  case '!':
    AddToken(Match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    break;
  case '=':
    if(Match('=')) AddToken(TOKEN_EQUAL_EQUAL);
    else if(Match('>')) AddToken(TOKEN_ARROW);
    else AddToken(TOKEN_EQUAL);
    return;
  case '<':
    AddToken(Match('=') ? TOKEN_LESSER_EQUAL : TOKEN_LESSER);
    break;
  case '>':
    AddToken(Match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    break;
  case '/':
    if (Match('/')) {
      while (Peek() != '\n' && !AtEnd())
        Advance();
    } else if (Match('*')) {
      while ((Peek() != '*' || PeekNext() != '/') && !AtEnd()) {
        Advance();
      }

      // Ignorem el ultim '*/'
      Advance();
      Advance();
    } else {
      AddToken(TOKEN_SLASH);
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
    if (IsDigit(c))
      Number();
    else if (IsAlpha(c))
      Identifier();
    else
      Panic(Error(current, 1, std::string("Unexpected character '") + c + "'",
                  fileName));
  }
}

void Scanner::AddToken(TokenType type) { AddToken(type, ""); }

void Scanner::AddToken(TokenType type, std::string content) {
  std::string text = source.substr(start, current - start);
  tokens->push_back(Token(type, text, content, start + 1));
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

  Advance();
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
    Panic(Error(start, current - start, "String not terminated", fileName));
    return;
  }

  // The " remaining
  Advance();

  AddToken(TOKEN_STRING, source.substr(start + 1, current - start - 2));
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

  AddToken(TOKEN_NUMBER, source.substr(start, current - start));
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
  if (type)
    type = TOKEN_IDENTIFIER;

  AddToken(type);
}

std::unordered_map<std::string, TokenType> Scanner::keywords{
    {"if", TOKEN_IF},       {"else", TOKEN_ELSE}, {"elif", TOKEN_ELIF},
    {"for", TOKEN_FOR},     {"fn", TOKEN_FN},     {"this", TOKEN_THIS},
    {"super", TOKEN_SUPER}, {"none", TOKEN_NONE}, {"ret", TOKEN_RETURN},
    {"class", TOKEN_CLASS}, {"true", TOKEN_TRUE}, {"false", TOKEN_FALSE}};
