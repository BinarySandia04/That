#include "nodes.h"
#include <iostream>

using namespace That;

Nodes::Node::Node(NodeType type){
    this->type = type;
}

Nodes::NodeType Nodes::Node::GetType(){
    return this->type;
}

Nodes::Error::Error(int errorCode) : Node(NodeType::NODE_ERROR){
    this->errorCode = errorCode;
}

int Nodes::Error::GetErrorCode(){
    return this->errorCode;
}


Nodes::Expression::Expression(ExpressionType type){
    this->expType = type;
}

Nodes::Expression::Expression(Nodes::Literal value){
    this->value = value;
    this->expType = Nodes::ExpressionType::VALUE;
}

Nodes::Expression::Expression(){
    this->expType = ExpressionType::VALUE;
}

Nodes::ExpressionType Nodes::Expression::GetType(){
    return this->expType;
}

Nodes::Literal::Literal(std::string value, LiteralType type){
    this->value = value;
    this->type = type;
}

Nodes::Literal::Literal(){
    this->type = LiteralType::VOID;
}

std::string Nodes::Literal::GetValue(){
    return value;
}

Nodes::Literal::LiteralType Nodes::Literal::GetLiteralType(){
    return type;
}

Nodes::Binary::Binary(Expression *first, Token::TokenType operation, Expression *second) : Expression(ExpressionType::BINARY){
    this->first = first;
    this->operation = operation;
    this->second = second;
}

Nodes::Unary::Unary(Expression *expression, Token::TokenType operation) : Expression(ExpressionType::UNARY){
    this->first = expression;
    this->operation = operation;
}

Nodes::Call::Call(std::vector<Expression*> args, std::string function) : Expression(ExpressionType::CALL){
    this->args = args;
    this->function = function;
}

Nodes::Declaration::Declaration(Token::TokenType variableType, std::string varName) : Node(NodeType::DECLARATION){
    this->variableType = variableType;
    this->varName = varName;
}

Nodes::Assignation::Assignation(std::string varName, Expression *expression) : Node(NodeType::ASSIGNATION){
    this->varName = varName;
    this->expression = expression;
}

Nodes::If::If(Expression *condition, std::vector<Node*> ifChildren, std::vector<Node*> elseChildren) : Node(NodeType::IF){
    this->condition = condition;
    this->ifChildren = ifChildren;
    this->elseChildren = elseChildren;
}

Nodes::While::While(Expression *condition, std::vector<Node*> children) : Node(NodeType::WHILE){
    this->condition = condition;
    this->children = children;
}

Nodes::Function::Function(std::string name, std::vector<Declaration*> arguments, std::vector<Node*> children, Literal::LiteralType returnType) : Node(NodeType::FUNCTION){
    this->name = name;
    this->arguments = arguments;
    this->children = children;
    this->returnType = returnType;
}