#include <initializer_list>
#include <iostream>

#include "parser.h"
#include "token.h"

using namespace Zag;

Parser::Parser(std::string fileName) {
  this->fileName = fileName;
  current = 0;
  panic = false;
}

Node::Node() { this->type = NODE_UNDEF; }

Node::Node(NodeType type) { this->type = type; }

Node::Node(NodeType type, std::string data) {
  this->type = type;
  this->data = data;
}

void Node::Debug(int tabs) {
  PrintTabs(tabs);
  std::cout << "{" << std::endl;
  PrintTabs(tabs);
  std::cout << "type: " << type << std::endl;
  PrintTabs(tabs);
  std::cout << "data: " << data << std::endl;
  PrintTabs(tabs);
  std::cout << "childs: " << children.size() << std::endl;
  if (children.size() > 0) {
    PrintTabs(tabs);
    std::cout << "children: [" << std::endl;
    for (int i = 0; i < children.size(); i++) {
      children[i]->Debug(tabs + 1);
    }
  }
  PrintTabs(tabs);
  std::cout << "]}" << std::endl;
}

void Node::PrintTabs(int tabs) {
  for (int i = 0; i < tabs; i++)
    std::cout << "\t";
}

Token Parser::Peek() {
  if (current < tokens->size())
    return (*tokens)[current];
  else
    return (*tokens)[tokens->size() - 1];
}

TokenType Parser::PeekType() { return Peek().type; }

Token Parser::Advance() {
  if (current < tokens->size()) {
    current++;
    return Peek();
  } else
    return (*tokens)[tokens->size() - 1];
}

TokenType Parser::AdvancePeekType() {
  if (current + 1 >= tokens->size())
    return TokenType::TOKEN_END_OF_FILE;
  return (*tokens)[current + 1].type;
}

bool Parser::Match(TokenType type) {
  if (PeekType() == type) {
    Advance();
    return true;
  }
  return false;
}

bool Parser::MatchAny(std::initializer_list<TokenType> types) {
  for (TokenType type : types) {
    if (Match(type))
      return true;
  }
  return false;
}

TokenType Parser::PreviousType() {
  if (current - 1 < 0)
    return TokenType::TOKEN_END_OF_FILE;
  return (*tokens)[current - 1].type;
}

void Parser::EnterPanic(int offset, int size, std::string content) {
  panic = true;
  error = Error(offset, size, content, fileName);
}

void Parser::EnterPanic(Token token, std::string content) {
  if(token.type == TOKEN_END_OF_FILE) EnterPanic(token.offset, 1, content);
  EnterPanic(token.offset, token.lexeme.size(), content);
}

void Parser::Expect(TokenType type, std::string content) {
  if (PeekType() != type)
    EnterPanic(Peek(), content);
  else
    Advance();
}

bool Parser::AtEnd(){
  return current >= tokens->size() || PeekType() == TOKEN_END_OF_FILE;
}

bool Parser::GenerateAST(std::vector<Token> *tokens, Node **tree, Error *err) {
  this->tokens = tokens;
  PopulateSpace(tree);

  if (panic) {
    // Free root memory?
    *err = error;
    return false;
  }

  return true;
}

void Parser::PopulateSpace(Node **root) {
  while (PeekType() != TOKEN_END_OF_FILE) {
    Consume(root);
    if (panic)
      return;
  }
}

void Parser::ConsumeEmpty(){
  while(Match(TOKEN_SEMICOLON));
}

void Parser::Consume(Node **block) {
  Node *line = new Node();

  Statement(&line);

  ConsumeEmpty();

  (*block)->children.push_back(line);
}

void Parser::Block(Node **block){
  (*block)->type = NODE_BLOCK;
  Expect(TOKEN_LEFT_BRACE, "Expected '{' at the start of a block");

  while(!Match(TOKEN_RIGHT_BRACE)){
    if(AtEnd()){
      EnterPanic(Peek(), "Expected '}' for closing block");
      return;
    }
    Consume(block);
    if(panic) return;
  }
}

void Parser::Statement(Node **node){
  if(Match(TOKEN_IF)){
    If(node);
    return;
  }

  Expression(node);

}

void Parser::If(Node **node){
  // We already matched if. Now we consume the expression and the if block
  (*node)->type = NODE_IF;

  Node *condition = new Node();
  Expression(&condition);

  Node *block = new Node();
  Block(&block);

  (*node)->children.push_back(block); 
  (*node)->arguments.push_back(condition);

  // We match else if
  while(Match(TOKEN_ELIF)){
    condition = new Node();
    Expression(&condition);

    block = new Node();
    Block(&block);

    (*node)->arguments.push_back(condition);
    (*node)->children.push_back(block);
  }

  // If there is an else we also catch it
  if(Match(TOKEN_ELSE)){
    block = new Node();
    Block(&block);
    (*node)->children.push_back(block);
  }
}

void Parser::For(Node **node){

}

void Parser::Primary(Node **exp) {
  Token t = Peek();
  switch (t.type) {
  case TOKEN_TRUE:
    (*exp)->type = NODE_TRUE_VAL;
    Advance();
    break;
  case TOKEN_FALSE:
    (*exp)->type = NODE_FALSE_VAL;
    Advance();
    break;
  case TOKEN_NONE:
    (*exp)->type = NODE_NONE_VAL;
    Advance();
    break;
  case TOKEN_NUMBER:
    (*exp)->type = NODE_NUMBER_VAL;
    (*exp)->data = t.literal;
    Advance();
    break;
  case TOKEN_STRING:
    (*exp)->type = NODE_STRING_VAL;
    (*exp)->data = t.literal;
    Advance();
    break;
  case TOKEN_LEFT_PAREN: {
    // Nested expression
    Advance();
    Expression(exp);
    Expect(TOKEN_RIGHT_PAREN, "Unclosed right parenthesis");
    break;
  }
  default:
    EnterPanic(t, "Unexpected primary type");
    break;
  }
}

void Parser::Unary(Node **exp) {
  if (MatchAny({TOKEN_BANG, TOKEN_MINUS})) {
    TokenType opToken = PreviousType();
    (*exp)->type = NODE_OP_UN;

    switch (opToken) {
    case TOKEN_BANG:
      (*exp)->data = "!";
      break;
    default:
      (*exp)->data = "-";
      break;
    }

    Node *operand = new Node(NODE_EXPRESSION);

    Primary(&operand);
    (*exp)->children.push_back(operand);

  } else {
    Primary(exp);
  }
}

void Parser::Factor(Node **exp) {
  Token t = Peek();
  // Aixo serà un unary si o si
  Unary(exp);

  while (MatchAny({TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENTAGE})) {
    Node *factor, *second;
    TokenType opToken = PreviousType();

    switch (opToken) {
    case TOKEN_STAR:
      factor = new Node(NODE_OP_BIN, "*");
      break;
    case TOKEN_SLASH:
      factor = new Node(NODE_OP_BIN, "/");
      break;
    default:
      factor = new Node(NODE_OP_BIN, "%");
      break;
    }

    factor->children.push_back(*exp);

    second = new Node(NODE_EXPRESSION);
    Unary(&second);

    factor->children.push_back(second);
    *exp = factor;
  }
}

void Parser::Term(Node **exp) {
  Token t = Peek();
  Factor(exp);

  while (MatchAny({TOKEN_PLUS, TOKEN_MINUS})) {

    Node *term, *second;
    TokenType opToken = PreviousType();

    switch (opToken) {
    case TOKEN_PLUS:
      term = new Node(NODE_OP_BIN, "+");
      break;
    default:
      term = new Node(NODE_OP_BIN, "-");
      break;
    }

    term->children.push_back(*exp);

    second = new Node(NODE_EXPRESSION);
    Factor(&second);

    term->children.push_back(second);

    *exp = term;
  }
}

void Parser::Comparison(Node **exp) {
  Token t = Peek();
  Term(exp);

  while (MatchAny(
      {TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESSER, TOKEN_LESSER_EQUAL})) {
    Node *comparison, *second;
    TokenType opToken = PreviousType();

    switch (opToken) {
    case TOKEN_GREATER:
      comparison = new Node(NODE_OP_BIN, ">");
      break;
    case TOKEN_GREATER_EQUAL:
      comparison = new Node(NODE_OP_BIN, ">=");
      break;
    case TOKEN_LESSER:
      comparison = new Node(NODE_OP_BIN, "<");
      break;
    default:
      comparison = new Node(NODE_OP_BIN, "<=");
      break;
    }

    comparison->children.push_back(*exp);

    second = new Node(NODE_EXPRESSION);
    Term(&second);

    comparison->children.push_back(second);
    *exp = comparison;
  }
}

void Parser::Equality(Node **exp) {
  Token t = Peek();
  Comparison(exp);

  while (MatchAny({TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL})) {
    Node *equality, *second;
    TokenType opToken = PreviousType();

    switch (opToken) {
    case TOKEN_EQUAL_EQUAL:
      equality = new Node(NODE_OP_BIN, "==");
      break;
    default:
      equality = new Node(NODE_OP_BIN, "!=");
      break;
    }

    equality->children.push_back(*exp);

    second = new Node(NODE_EXPRESSION);
    Term(&second);

    equality->children.push_back(second);
    *exp = equality;
  }
}

void Parser::Expression(Node **exp) { Equality(exp); }
