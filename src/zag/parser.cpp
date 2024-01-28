#include <initializer_list>
#include <iostream>

#include "node.h"
#include "parser.h"
#include "token.h"

using namespace Zag;

Parser::Parser(std::string fileName) {
  this->fileName = fileName;
  current = 0;
  panic = false;
}

Token Parser::Peek() { return PeekN(0); }

TokenType Parser::PeekType() { return PeekNType(0); }

Token Parser::PeekN(int n) {
  if (current + n < tokens->size())
    return (*tokens)[current + n];
  else if (current + n < 0)
    return (*tokens)[0];
  else
    return (*tokens)[tokens->size() - 1];
}

TokenType Parser::PeekNType(int n) { return PeekN(n).type; }

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
  if (token.type == TOKEN_END_OF_FILE)
    EnterPanic(token.offset, 1, content);
  EnterPanic(token.offset, token.lexeme.size(), content);
}

void Parser::Expect(TokenType type, std::string content) {
  if (PeekType() != type)
    EnterPanic(Peek(), content);
  else
    Advance();
}

void Parser::Panic(std::string content) { EnterPanic(PeekN(-1), content); }

bool Parser::AtEnd() {
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

void Parser::ConsumeEmpty() {
  while (Match(TOKEN_SEMICOLON))
    ;
}

void Parser::Consume(Node **block) {
  Node *line = new Node();

  Statement(&line);

  ConsumeEmpty();

  (*block)->children.push_back(line);
}

void Parser::Block(Node **block) {
  (*block)->type = NODE_BLOCK;

  // Expect(TOKEN_LEFT_BRACE, "Expected '{' at the start of a block");

  if (Match(TOKEN_LEFT_BRACE)) {
    while (!Match(TOKEN_RIGHT_BRACE)) {
      if (AtEnd()) {
        EnterPanic(Peek(), "Expected '}' for closing block");
        return;
      }
      Consume(block);
      if (panic)
        return;
    }
  } else if (Match(TOKEN_DOUBLE_DOTS)) {
    if (AtEnd()) {
      EnterPanic(Peek(), "Expected statement after one-liner block");
      return;
    }
    Consume(block);
    if (panic)
      return;
  } else if (Match(TOKEN_SEMICOLON)) {
    // Empty block
    return;
  } else {
    EnterPanic(Peek(), "Expected '{' or ':' at the start of block");
  }
}

void Parser::Statement(Node **node) {
  if (Match(TOKEN_IF)) {
    If(node);
    return;
  }
  if (Match(TOKEN_LUP)) {
    Lup(node);
    return;
  }
  if (Match(TOKEN_FN)) {
    Fn(node);
    return;
  }
  if (Match(TOKEN_RET)) {
    Ret(node);
    return;
  }
  if(Match(TOKEN_BRK)){
    Brk(node);
    return;
  }
  if(Match(TOKEN_GET)){
    Get(node);
    return;
  }
  if (CheckAssignation()) {
    Assignation(node);
    return;
  }

  Expression(node);
}

void Parser::Assignation(Node **assignation) {
  (*assignation)->type = NODE_ASSIGNATION;

  // 2 childs: var name and value
  // 1 arg: declared type

  // Passed after CheckAssignation ==> TOKEN_IDENTIFIER, TOKEN_EQUAL, ...
  //                                         ^
  //                                         |
  bool typed = false;

  Node *identifier = new Node(NODE_IDENTIFIER);
  (*identifier).data = Peek().literal;
  (*assignation)->children.push_back(identifier);

  Advance();
  // We check if the assignation is typed
  if (Match(TOKEN_DOUBLE_DOTS)) {
    Node *type = new Node(NODE_TYPE);
    Type(&type);
    (*assignation)->arguments.push_back(type);
    typed = true;
  }

  // If it is not, we must provide an equality
  if (!typed) {
    Expect(TOKEN_EQUAL, "Expected '='");
    if (panic)
      return;
  }
  // It if is typed and we do not have an equal we ended the statement
  if (typed) {
    if (PeekType() != TOKEN_EQUAL) {
      return;
    }
    Advance();
  }

  // Now we expect an expression afer the equal
  Node *expression = new Node(NODE_EXPRESSION);
  Expression(&expression);

  (*assignation)->children.push_back(expression);
}

void Parser::If(Node **node) {
  // We already matched if. Now we consume the expression and the if block
  (*node)->type = NODE_IF;

  Node *condition = new Node();
  Expression(&condition);

  Node *block = new Node();
  Block(&block);

  (*node)->children.push_back(block);
  (*node)->arguments.push_back(condition);

  // We match else if
  while (Match(TOKEN_EIF)) {
    condition = new Node();
    Expression(&condition);

    block = new Node();
    Block(&block);

    (*node)->arguments.push_back(condition);
    (*node)->children.push_back(block);
  }

  // If there is an else we also catch it
  if (Match(TOKEN_ELS)) {
    block = new Node();
    Block(&block);
    (*node)->children.push_back(block);
  }
}

bool Parser::CheckBlock() {
  return PeekType() == TOKEN_LEFT_BRACE || PeekType() == TOKEN_DOUBLE_DOTS ||
         PeekType() == TOKEN_SEMICOLON;
}

bool Parser::CheckAssignation() {
  return PeekType() == TOKEN_IDENTIFIER &&
         (PeekNType(1) == TOKEN_EQUAL || PeekNType(1) == TOKEN_DOUBLE_DOTS);
}

bool Parser::CheckIterator() {
  // IDENTIFIER COMMA IDENTIFIER COMMA IDENTIFIER .... PIPE
  int a = 0;
  while (PeekNType(a) == TOKEN_IDENTIFIER) {
    a += 1;
    if (PeekNType(a) == TOKEN_END_OF_FILE)
      Panic("Unexpected EOF");

    if (PeekNType(a) != TOKEN_COMMA && PeekNType(a) != TOKEN_PIPE)
      return false;
    if (PeekNType(a) == TOKEN_PIPE)
      return true;
    a += 1;

    if (PeekNType(a) == TOKEN_END_OF_FILE)
      Panic("Unexpected EOF");
  }
  return false;
}

void Parser::Lup(Node **lup) {
  // Assume that we matched it
  (*lup)->type = NODE_LUP;

  (*lup)->data = "";
  // We get the lup name
  if (PeekType() == TOKEN_CONST) {
    (*lup)->data = Peek().literal;
    Advance();
  }

  if (CheckIterator()) {
    // iterator form
    Node *iterators = new Node(NODE_LUP_ITERATORS);
    LupIterators(&iterators);
    (*lup)->arguments.push_back(iterators);
  } else if (!CheckBlock()) {
    // while form
    // We have our expression
    if (CheckAssignation()) {
      // for mode
      Node *n = new Node(NODE_ASSIGNATION);
      Assignation(&n);
      (*lup)->arguments.push_back(n);

      Expect(TOKEN_PIPE, "Expected '|' after lupfor initial assignation");

      n = new Node(NODE_EXPRESSION);
      Expression(&n);
      (*lup)->arguments.push_back(n);

      Expect(TOKEN_PIPE, "Expected '|' after lupfor conditional expression");

      n = new Node(NODE_ASSIGNATION);
      Assignation(&n);
      (*lup)->arguments.push_back(n);
    } else {
      Node *exp = new Node(NODE_EXPRESSION);
      Expression(&exp);

      (*lup)->arguments.push_back(exp);
    }
  }

  // We get the inner block
  Node *block = new Node();
  Block(&block);

  (*lup)->children.push_back(block);
}

// TODO
void Parser::Fn(Node **fun) {
  (*fun)->type = NODE_FUNCTION;
  // First of all we expect an identifier
  if (PeekType() != TOKEN_IDENTIFIER) {
    Panic("Expected an identifier after 'fn'");
    return;
  }

  (*fun)->data = Peek().literal;
  Advance();

  // We expect an argument list
}

void Parser::Ret(Node **ret) {
  (*ret)->type = NODE_RET;
  // As Lua does, we expect the following expressions to be for the return
  std::cout << Peek().lexeme << std::endl;
  if (PeekType() == TOKEN_RIGHT_BRACE)
    return; // No chance for confusion here
  do {
    Node *exp = new Node(NODE_EXPRESSION);
    Expression(&exp);
    (*ret)->children.push_back(exp);
  } while (Match(TOKEN_COMMA));
}

void Parser::Brk(Node **brk){
  (*brk)->type = NODE_BRK;

  if (PeekType() == TOKEN_CONST) {
    (*brk)->data = Peek().literal;
    Advance();
  }

  if(Match(TOKEN_IF)){
    Node* exp = new Node(NODE_EXPRESSION);
    Expression(&exp);
    (*brk)->children.push_back(exp);
  }
}

// TODO
void Parser::Kin(Node **kin) {}

void Parser::Get(Node **get) {
  (*get)->type = NODE_GET;
  // We put lexemes because we want to distinct const and string (local and global)
  if(PeekType() == TOKEN_CONST){
    (*get)->data = Peek().lexeme;
    Advance();
  }
  else if(PeekType() == TOKEN_STRING){
    (*get)->data = Peek().lexeme;
    Advance();
  } else {
    Panic("Expected constant or string for get");
  }
}

void Parser::Type(Node **type) {
  (*type)->type = NODE_TYPE;

  if (PeekType() == TOKEN_STAR) {
    (*type)->data = "*";
    Advance();
    return;
  }

  if (PeekType() == TOKEN_IDENTIFIER) {
    (*type)->data = Peek().literal;
    Advance();
    // Now we get recursive type defs ( i.e List<Int> )

    if (Match(TOKEN_LESSER)) {
      if (Match(TOKEN_GREATER)) {
        Panic("Expected type inside <>");
        return;
      }
      do {
        if (PeekType() == TOKEN_IDENTIFIER || PeekType() == TOKEN_STAR) {
          Node *subType = new Node;
          bool isStar = PeekType() == TOKEN_STAR;
          Type(&subType);

          (*type)->children.push_back(subType);
          if (!isStar)
            Advance();
        }
        if (AtEnd()) {
          Panic("Reached end of file before closing type definition");
          return;
        }

        if (Match(TOKEN_GREATER)) {
          break;
        }
      } while (Match(TOKEN_COMMA));

      // All good if we get here
      return;
    }
  } else {
    Panic("Expected type identifier");
  }
}

void Parser::Primary(Node **exp) {
  Token t = Peek();
  switch (t.type) {
  case TOKEN_YEP:
    (*exp)->type = NODE_YEP_VAL;
    Advance();
    break;
  case TOKEN_NOP:
    (*exp)->type = NODE_NOP_VAL;
    Advance();
    break;
  case TOKEN_NIL:
    (*exp)->type = NODE_NIL_VAL;
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
  case TOKEN_IDENTIFIER:
    (*exp)->type = NODE_IDENTIFIER;
    (*exp)->data = t.literal;
    Advance();
    break;
  case TOKEN_LEFT_BRACKET:
    Advance();
    Array(exp);
    break;
  case TOKEN_LEFT_PAREN: {
    // Nested expression
    Advance();
    Expression(exp);
    Expect(TOKEN_RIGHT_PAREN, "Unclosed right parenthesis");
    break;
  }
  default:
    Panic("Unexpected primary type");
    break;
  }
}

void Parser::Array(Node **array) {
  (*array)->type = NODE_ARRAY;
  while (PeekType() != TOKEN_RIGHT_BRACKET) {
    Node *exp = new Node();
    Expression(&exp);
    (*array)->children.push_back(exp);

    if (!Match(TOKEN_COMMA)) {
      if (PeekType() == TOKEN_RIGHT_BRACKET) {
        Advance();
        return;
      }
    }

    if (AtEnd()) {
      Panic("Unclosed right bracket for listable");
      return;
    }
  }
}

void Parser::LupIterators(Node **iterators) {
  (*iterators)->type = NODE_LUP_ITERATORS;
  // We have guaranteed the following
  // ID COMMA ID COMMA ID .... PIPE
  // We save identifiers on args
  do {
    Node *identifier = new Node(NODE_IDENTIFIER);
    identifier->data = Peek().literal;
    (*iterators)->arguments.push_back(identifier);
    Advance();
  } while (Match(TOKEN_COMMA));

  // Now we consume intervals (do-while because we have a guaranteed pipe)
  while (Match(TOKEN_PIPE)) {
    Node *interval = new Node(NODE_INTERVAL);
    Interval(&interval);
    (*iterators)->children.push_back(interval);
  }
}

void Parser::Interval(Node **interval) {
  (*interval)->type = NODE_INTERVAL;
  // We get the first expression
  Node *exp = new Node(NODE_EXPRESSION);
  Expression(&exp);
  (*interval)->children.push_back(exp);

  // If we have comma we consume a second one
  if (Match(TOKEN_COMMA)) {
    exp = new Node(NODE_EXPRESSION);
    Expression(&exp);
    (*interval)->children.push_back(exp);
  }
}

void Parser::Unary(Node **exp) {
  if (MatchAny({TOKEN_BANG, TOKEN_MINUS, TOKEN_SLASH})) {
    TokenType opToken = PreviousType();
    (*exp)->type = NODE_OP_UN;

    switch (opToken) {
    case TOKEN_BANG:
      (*exp)->data = "!";
      break;
    case TOKEN_SLASH:
      (*exp)->data = "/";
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
