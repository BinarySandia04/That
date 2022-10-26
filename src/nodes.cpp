#include "nodes.h"

using namespace Glass;

void Nodes::Expression::Evaluate(){

}

Nodes::Literal::Literal(std::string value, LiteralType type){
    this->value = value;
    this->type = type;
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

Nodes::Declaration::Declaration(Token::TokenType variableType, std::string varName){
    this->variableType = variableType;
    this->varName = varName;
}

Nodes::Assignation::Assignation(std::string varName, Expression expression){
    this->varName = varName;
    this->expression = expression;
}

Nodes::If::If(Expression condition, std::vector<Node*> ifChildren, std::vector<Node*> elseChildren){
    this->condition = condition;
    this->ifChildren = ifChildren;
    this->elseChildren = elseChildren;
}

Nodes::While::While(Expression condition, std::vector<Node*> children){
    this->condition = condition;
    this->children = children;
}

Nodes::Function::Function(std::string name, std::vector<Declaration> arguments, std::vector<Node*> children, Literal::LiteralType returnType){
    this->name = name;
    this->arguments = arguments;
    this->children = children;
    this->returnType = returnType;
}