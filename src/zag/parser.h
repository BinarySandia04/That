#pragma once

#include <vector>

#include "token.h"
#include "error.h"

namespace Zag {

enum NodeType {
  NODE_SPACE,
  NODE_FUNCTION,
  NODE_OP_BIN,
  NODE_OP_UN,
  NODE_ASSIGN,
  NODE_LITERAL,
  LITERAL,
};

enum NodeFunction {
  NODE_EXPRESSION,
  NODE_BLOCK
};

class Expression {};

class BinaryExpression : Expression {};

class Node {
  public:
    NodeType type;
    NodeFunction function;

    std::vector<Node*> children;
    std::vector<Node*> arguments;

    std::string data;

    Node(NodeType, NodeFunction);
    Node(NodeType, NodeFunction, std::string);
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
  Token Advance();

  bool Match(TokenType);
  void EnterPanic(int, int, std::string);
  void EnterPanic(Token, std::string);
  void Expect(TokenType, std::string);

  void PopulateSpace(Node*);
  void Consume(Node *);

  std::vector<Token> *tokens;
};

} // namespace Zag
