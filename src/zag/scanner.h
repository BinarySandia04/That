#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "token.h"

namespace Zag {

class Scanner {
public:
  Scanner(std::string code);
  void ScanTokens(std::vector<Token> *tokens);

private:
  std::vector<Token> *tokens;
  std::string source;

  static std::unordered_map<std::string, TokenType> keywords;

  int start;
  int current;

  bool AtEnd();
  char Advance();
  void AddToken(TokenType);
  void AddToken(TokenType, std::string);
  void ScanToken();
  bool Match(char);
  char Peek();
  char PeekNext();
  void GetString();
  bool IsDigit(char);
  bool IsDigitNumber(char);
  void Number();
  bool IsAlpha(char);
  bool IsAlphaNumeric(char);
  void Identifier();
};

} // namespace Zag
