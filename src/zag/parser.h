#pragma once

#include <vector>
#include <initializer_list>

#include "token.h"
#include "error.h"
#include "node.h"

namespace Zag {

class Parser {
public:
  Parser(std::string);
  bool GenerateAST(std::vector<Token>*, Node**, Error*);
private:
  int current;
  bool panic;
  Error error;

  std::string fileName;

  TokenType PeekType();
  Token Peek();
  TokenType PeekNType(int);
  Token PeekN(int);

  TokenType AdvancePeekType();

  TokenType PreviousType();

  Token Advance();

  bool Match(TokenType);
  bool MatchAny(std::initializer_list<TokenType>);

  void ConsumeEmpty();
  void EnterPanic(int, int, std::string);
  void EnterPanic(Token, std::string);
  void Panic(std::string);
  void Expect(TokenType, std::string);
  bool AtEnd();

  bool CheckBlock();
  bool CheckAssignation();
  bool CheckIncrementor(); 
  bool CheckIterator();
  bool IsAssignationType(TokenType);

  void PopulateSpace(Node**);
  void Consume(Node **);

  void Type(Node **);
  void Primary(Node **);
  void Call(Node **);
  void Unary(Node **);
  void Factor(Node **);
  void Term(Node **);
  void Comparison(Node **);
  void Equality(Node **);
  void Expression(Node **);
  void Array(Node **);
  void LupIterators(Node **);
  void Interval(Node **);

  void Assignation(Node **);
  void Statement(Node **);
  void If(Node **);
  void Lup(Node **);
  void Block(Node **);
  void Fn(Node **);
  void Args(Node **);
  void Ret(Node **);
  void Brk(Node **);
  void Kin(Node **);
  void Get(Node **);

  std::vector<Token> *tokens;
};

} // namespace Zag
