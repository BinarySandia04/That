#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Ast/token.h"
#include "Frontend/error.h"

namespace ZagIR {

class Scanner {
public:
  Scanner(std::string, std::string);
  bool ScanTokens(std::vector<Token> *tokens, Error *error);

private:
  std::vector<Token> *tokens;
  std::string source;
  std::string fileName;

  Error error;
  bool panic;

  static std::unordered_map<std::string, TokenType> keywords;

  int start;
  int current;

  int line;
  int column;

  bool AtEnd();
  char Advance();
  void AddToken(TokenType);
  void AddToken(TokenType, std::string);
  void ScanToken();
  bool Match(char);
  char Peek();
  char PeekNext();
  void GetString();
  void GetConstant();
  bool ValidConst(char);
  bool IsDigit(char);
  bool IsDigitNumber(char);
  bool IsEmpty(char);
  void Number();
  bool IsAlpha(char);
  bool IsAlphaNumeric(char);
  void Identifier();

  void Panic(Error);
};

} // namespace Zag
