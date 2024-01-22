#pragma once

#include <vector>
#include <initializer_list>

#include "token.h"
#include "error.h"

namespace Zag {

enum NodeType {
  NODE_SPACE,
  NODE_FUNCTION,
  NODE_OP_BIN,
  NODE_OP_UN,
  NODE_ASSIGN,
  NODE_EXPRESSION,
  NODE_TRUE_VAL,
  NODE_FALSE_VAL,
  NODE_NONE_VAL,
  NODE_NUMBER_VAL,
  NODE_STRING_VAL,
};

class Node {
  public:
    NodeType type;

    std::vector<Node*> children;
    std::vector<Node*> arguments;

    std::string data;

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

  TokenType AdvancePeekType();

  TokenType PreviousType();

  Token Advance();

  bool Match(TokenType);
  bool MatchAny(std::initializer_list<TokenType>);

  void EnterPanic(int, int, std::string);
  void EnterPanic(Token, std::string);
  void Expect(TokenType, std::string);

  void PopulateSpace(Node**);
  void Consume(Node **);


  void Primary(Node **);
  void Unary(Node **);
  void Factor(Node **);
  void Term(Node **);
  void Comparison(Node **);
  void Equality(Node **);
  void Expression(Node **);

  std::vector<Token> *tokens;
};

} // namespace Zag
