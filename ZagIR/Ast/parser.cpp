#include <ZagIR/Ast/token.h>
#include <initializer_list>
#include <iostream>

#include "node.h"
#include "parser.h"
#include "token.h"

using namespace ZagIR;

/*
 * TODO: Falten:
 * - hoc
 * - sintaxis hoc per cosa extra
 * - constructors
 */

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

bool Parser::MatchWithoutSeparator(TokenType type) {
  if (PeekType() == type && !Peek().separator) {
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

void Parser::Consume(Node **block) {
  Node *line = new Node();

  Statement(&line);

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
  if (Match(TOKEN_BRK)) {
    Brk(node);
    return;
  }
  if (Match(TOKEN_GET)) {
    Get(node);
    return;
  }
  if (Match(TOKEN_KIN)) {
    Kin(node);
    return;
  }

  Assignation(node);
}

void Parser::Assignation(Node **assignation) {

  Expression(assignation);
  if (!(IsAssignationOrIncrementor(PeekType())) &&
      PeekType() != TOKEN_DOUBLE_DOTS) {
    return;
  }

  Node *trueAssignation = new Node(NODE_ASSIGNATION);
  trueAssignation->children.push_back(*assignation);
  *assignation = trueAssignation;

  // 2 childs: var name and value
  // 1 arg: declared type

  bool typed = false;
  // We check if the assignation is typed
  if (Match(TOKEN_DOUBLE_DOTS)) {
    Node *type = new Node(NODE_TYPE);
    Type(&type);
    (*assignation)->arguments.push_back(type);
    typed = true;
  }

  // If it is not, we must provide an equality
  if (!typed) {
    if (MatchAny({TOKEN_EQUAL, TOKEN_MINUS_EQUAL, TOKEN_PLUS_EQUAL,
                  TOKEN_STAR_EQUAL, TOKEN_SLASH_EQUAL,
                  TOKEN_PERCENTAGE_EQUAL})) {
      switch (PreviousType()) {
      case TOKEN_EQUAL:
        (*assignation)->data = "=";
        break;
      case TOKEN_MINUS_EQUAL:
        (*assignation)->data = "-=";
        break;
      case TOKEN_PLUS_EQUAL:
        (*assignation)->data = "+=";
        break;
      case TOKEN_STAR_EQUAL:
        (*assignation)->data = "*=";
        break;
      case TOKEN_SLASH_EQUAL:
        (*assignation)->data = "/=";
        break;
      case TOKEN_PERCENTAGE_EQUAL:
        (*assignation)->data = "%=";
        break;
      default:
        Panic("Expected valid assignation symbol");
        return;
      }
    } else {
      Panic("Expected assignation symbol");
      return;
    }
  }
  // It if is typed and we do not have an equal we ended the statement
  if (typed) {
    if (PeekType() != TOKEN_EQUAL) {
      return;
    }
    (*assignation)->data = "=";
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

bool Parser::IsAssignationOrIncrementor(TokenType type) {
  return type == TOKEN_EQUAL || IsIncrementor(type);
}

bool Parser::IsIncrementor(TokenType type) {
  return type == TOKEN_PLUS_EQUAL || type == TOKEN_MINUS_EQUAL ||
         type == TOKEN_STAR_EQUAL || type == TOKEN_SLASH_EQUAL ||
         type == TOKEN_PERCENTAGE_EQUAL;
}

bool Parser::CheckIncrementor() {
  return PeekType() == TOKEN_IDENTIFIER && IsIncrementor(PeekNType(1));
}

bool Parser::IsAssignationType(TokenType t) {
  return (t == TOKEN_PLUS_EQUAL || t == TOKEN_MINUS_EQUAL ||
          t == TOKEN_STAR_EQUAL || t == TOKEN_SLASH_EQUAL ||
          t == TOKEN_PERCENTAGE_EQUAL || t == TOKEN_EQUAL);
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
    // std::cout << "Iterator" << std::endl;
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

void Parser::Fn(Node **fun) {
  (*fun)->type = NODE_FUNCTION;
  // First of all we expect an identifier
  if (PeekType() != TOKEN_IDENTIFIER) {
    Panic("Expected an identifier instead of keyword");
    return;
  }

  (*fun)->data = Peek().literal;
  Advance();

  // We expect a definition token or an arrow
  if (Match(TOKEN_LEFT_BRACKET)) {
    Node *args = new Node(NODE_ARGS);
    Args(&args);
    (*fun)->arguments.push_back((args));
  }

  if (Match(TOKEN_ARROW)) {
    Node *type = new Node(NODE_TYPE);
    Type(&type);
    (*fun)->arguments.push_back(type);
  }

  // We expect now the block
  Node *block = new Node(NODE_BLOCK);
  Block(&block);
  (*fun)->children.push_back(block);
}

void Parser::Args(Node **args) {
  (*args)->type = NODE_ARGS;

  while (PeekType() != TOKEN_RIGHT_BRACKET) {

    Node *arg = new Node(NODE_ARG);

    if (PeekType() != TOKEN_IDENTIFIER) {
      Panic("Expected identifier");
      return;
    }

    Node *identifier = new Node(NODE_IDENTIFIER);
    (*identifier).data = Peek().literal;
    arg->children.push_back(identifier);

    Advance();
    // We check if the assignation is typed
    if (Match(TOKEN_DOUBLE_DOTS)) {
      Node *type = new Node(NODE_TYPE);
      Type(&type);
      arg->arguments.push_back(type);
    }

    (*args)->children.push_back(arg);

    if (!Match(TOKEN_COMMA)) {
      if (PeekType() == TOKEN_RIGHT_BRACKET) {
        Advance();
        return;
      }
    }

    if (AtEnd()) {
      Panic("Unclosed right bracket for argument list");
      return;
    }
  }
}

void Parser::Ret(Node **ret) {
  (*ret)->type = NODE_RET;
  // As Lua does, we expect the following expressions to be for the return
  if (PeekType() == TOKEN_RIGHT_BRACE)
    return; // No chance for confusion here
  do {
    Node *exp = new Node(NODE_EXPRESSION);
    Expression(&exp);
    (*ret)->children.push_back(exp);
  } while (Match(TOKEN_COMMA));
}

void Parser::Brk(Node **brk) {
  (*brk)->type = NODE_BRK;

  if (PeekType() == TOKEN_CONST) {
    (*brk)->data = Peek().literal;
    Advance();
  }

  if (MatchWithoutSeparator(TOKEN_IF)) {
    Node *exp = new Node(NODE_EXPRESSION);
    Expression(&exp);
    (*brk)->children.push_back(exp);
  }
}

void Parser::Kin(Node **kin) {
  (*kin)->type = NODE_KIN;
  // We get the name of the kin
  if (PeekType() != TOKEN_IDENTIFIER) {
    Panic("Expected an identifier after 'kin'");
    return;
  }

  (*kin)->data = Peek().literal;
  Advance();

  // We expect a left brace
  Expect(TOKEN_LEFT_BRACE, "Expected '{' after class declaration");

  // std::cout << Peek().lexeme << std::endl;

  while (!Match(TOKEN_RIGHT_BRACE)) {
    if (AtEnd()) {
      EnterPanic(Peek(), "Expected '}' for closing block");
      return;
    }
    ConsumeKin(kin);
    if (panic)
      return;
  }
}

void Parser::ConsumeKin(Node **kin) {
  Node *kinEntry = new Node(NODE_KIN_ENTRY);

  KinEntry(&kinEntry);

  (*kin)->children.push_back(kinEntry);
}

void Parser::KinEntry(Node **kinStatement) {

  // Check for public/private accessors
  // static -> @, private -> $
  Node *kinEntryContent = new Node();
  if (Match(TOKEN_AT))
    (*kinStatement)->data = "@";
  else if (Match(TOKEN_DOLLAR))
    (*kinStatement)->data = "$";
  else
    (*kinStatement)->data = "";

  // Supongo que se puede añadir antes

  // We will get functions first
  if (Match(TOKEN_FN)) {
    Fn(&kinEntryContent);
    (*kinStatement)->children.push_back(kinEntryContent);
    return;
  }

  // Ok so we suppose that we have a variable then. If not we panic
  Assignation(&kinEntryContent);
  (*kinStatement)->children.push_back(kinEntryContent);
}

void Parser::Get(Node **get) {
  (*get)->type = NODE_GET;
  // We put lexemes because we want to distinct const and string (local and
  // global)
  if (PeekType() == TOKEN_CONST) {
    (*get)->data = Peek().lexeme;
    Advance();
  } else if (PeekType() == TOKEN_STRING) {
    (*get)->data = Peek().lexeme;
    Advance();
  } else {
    Panic("Expected constant or string for get");
  }
}

void Parser::Type(Node **type) {
  (*type)->type = NODE_TYPE;
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
        if (PeekType() == TOKEN_IDENTIFIER) {
          Node *subType = new Node;
          Type(&subType);

          (*type)->children.push_back(subType);
        }
        if (AtEnd()) {
          Panic("Reached end of file before closing type definition");
          return;
        }

        if (Match(TOKEN_GREATER)) {
          return;
        }
      } while (Match(TOKEN_COMMA));
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
    if ((*exp)->data.find(".") == std::string::npos)
      (*exp)->type = NODE_INT_VAL;
    else
      (*exp)->type = NODE_NUMBER_VAL;
    (*exp)->data = t.literal;
    Advance();
    break;
  case TOKEN_STRING:
    (*exp)->type = NODE_STRING_VAL;
    (*exp)->data = t.literal;
    Advance();
    break;
  case TOKEN_CHAR:
    (*exp)->type = NODE_CHAR_VAL;
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
    Panic("Unexpected primary type with lexeme " + t.lexeme);
    return;
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
  Expect(TOKEN_RIGHT_BRACKET, "Expected closing ']' at end of array");
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

    Call(&operand);
    (*exp)->children.push_back(operand);

  } else {
    // Primary(exp);
    Call(exp);
  }
}

void Parser::Call(Node **call) {
  // std::cout << "Calling primary at " << Peek().lexeme << std::endl;
  Primary(call);

  while (!AtEnd()) {
    if (Match(TOKEN_LEFT_PAREN)) {
      Node *c = new Node(NODE_CALL);
      // ogCall->type = NODE_CALL;

      // Get arguments until ')'
      while (PeekType() != TOKEN_RIGHT_PAREN) {
        Node *arg = new Node();
        Expression(&arg);
        c->arguments.push_back(arg);

        if (!Match(TOKEN_COMMA)) {
          if (PeekType() == TOKEN_RIGHT_PAREN) {
            break;
          }
        }

        if (AtEnd()) {
          Panic("Unclosed right parenthesis for function arguments");
          return;
        }
      }
      Advance();

      c->children.push_back(*call);
      *call = c;
    } else if (Match(TOKEN_LEFT_BRACKET)) {
      Node* accessor = new Node(NODE_ACCESSOR);

      accessor->children.push_back(*call);

      Node *index = new Node();
      Bool(&index);

      accessor->arguments.push_back(index);
      *call = accessor;

      Expect(TOKEN_RIGHT_BRACKET, "Expected ']' at end of accessor");
    } else if (Match(TOKEN_DOT)) {
      Node *getter = new Node(NODE_GETTER);
      Node *identifier = new Node(NODE_IDENTIFIER);

      identifier->data = Peek().literal;
      Advance();
      
      getter->arguments.push_back(identifier);
      getter->children.push_back(*call);
      *call = getter;
    } else {
      break;
    }
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

void Parser::Bool(Node **exp) {
  Token t = Peek();
  Equality(exp);

  while (MatchAny({TOKEN_AMP_AMP, TOKEN_PIPE_PIPE})) {
    Node *first, *second;
    TokenType opToken = PreviousType();

    switch (opToken) {
    case TOKEN_AMP_AMP:
      first = new Node(NODE_OP_BIN, "&&");
      break;
    default:
      first = new Node(NODE_OP_BIN, "||");
      break;
    }

    first->children.push_back(*exp);

    second = new Node(NODE_EXPRESSION);
    Equality(&second);

    first->children.push_back(second);
    *exp = first;
  }
}

void Parser::Expression(Node **exp) {
  (*exp)->type = NODE_EXPRESSION;
  Bool(exp);
}
