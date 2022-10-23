#include "nodes.h"

using namespace Glass;

void Nodes::Expression::Evaluate(){

}

Nodes::Binary::Binary(Expression first, Token::TokenType operation, Expression second){
    this->first = first;
    this->operation = operation;
    this->second = second;
}

Nodes::Unary::Unary(Expression expression, Token::TokenType operation){
    this->expression = expression;
    this->operation = operation;
}

Nodes::Call::Call(std::vector<Expression> args, std::string function){
    this->args = args;
    this->function = function;
}