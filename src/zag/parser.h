#pragma once

#include <vector>
#include <initializer_list>

#include "token.h"
#include "error.h"

namespace Zag {

enum NodeType {
  NODE_UNDEF,
  NODE_SPACE,
  NODE_BLOCK,
  NODE_FUNCTION,
  NODE_ARGS,
  NODE_IF,
  NODE_LUP,
  NODE_OP_BIN,
  NODE_OP_UN,
  NODE_ASSIGN,
  NODE_EXPRESSION,
  NODE_YEP_VAL,
  NODE_NOP_VAL,
  NODE_NIL_VAL,
  NODE_ASSIGNATION,
  NODE_TYPE,
  NODE_INTERVAL_ASSIGNATION, // children: 2 - 4, arguments: NODE_IDENTIFIER
  NODE_NUMBER_VAL,
  NODE_STRING_VAL,
  NODE_IDENTIFIER,
};

class Node {
  public:
    NodeType type;

    std::vector<Node*> children;
    std::vector<Node*> arguments;

    std::string data;

    Node();
    Node(NodeType);
    Node(NodeType, std::string);

    void Debug(int);
  private:
    void PrintTabs(int);
};

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
  bool CheckIterator();

  void PopulateSpace(Node**);
  void Consume(Node **);

  void Type(Node **);
  void Primary(Node **);
  void Unary(Node **);
  void Factor(Node **);
  void Term(Node **);
  void Comparison(Node **);
  void Equality(Node **);
  void Expression(Node **);

  void Assignation(Node **);
  void Statement(Node **);
  void If(Node **);
  void Lup(Node **);
  void Block(Node **);

  std::vector<Token> *tokens;
};

} // namespace Zag
