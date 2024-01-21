#include <iostream>

#include "parser.h"
#include "token.h"

using namespace Zag;

Parser::Parser(std::string fileName) {
  this->fileName = fileName;
  current = 0;
  panic = false;
}

Node::Node(NodeType type, NodeFunction function) {
  this->type = type;
  this->function = function;
}

Node::Node(NodeType type, NodeFunction function, std::string data) {
  this->type = type;
  this->function = function;
  this->data = data;
}

Token Parser::Peek() { return (*tokens)[current]; }

TokenType Parser::PeekType() { return Peek().type; }

Token Parser::Advance() {
  current++;
  return Peek();
}

bool Parser::Match(TokenType type) {
  if (PeekType() == type) {
    Advance();
    return true;
  }
  return false;
}

void Parser::EnterPanic(int offset, int size, std::string content) {
  panic = true;
  error = Error(offset, size, content, fileName);
}

void Parser::EnterPanic(Token token, std::string content) {
  EnterPanic(token.offset, token.lexeme.size(), content);
}

void Parser::Expect(TokenType type, std::string content) {
  if (PeekType() != type)
    EnterPanic(Peek(), content);
  else
    Advance();
}

bool Parser::GenerateAST(std::vector<Token> *tokens, Node **tree, Error *err) {
  this->tokens = tokens;
  PopulateSpace(*tree);

  if(panic){
    // Free root memory?
    *err = error;
    return false;
  }

  return true;
}

void Parser::PopulateSpace(Node *root) {
  while (PeekType() != TOKEN_END_OF_FILE) {
    Consume(root);
    if(panic) return;
  }
}

void Parser::Consume(Node *space) {
  Token t = Peek();
  switch (t.type) {
  
  default:
    EnterPanic(Peek(), "Unexpected token " + Peek().lexeme);
    break;
  }

  Advance();
}
