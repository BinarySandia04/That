#pragma once

#include <vector>
#include <string>

#include "lexer.h"

namespace Glass {
    namespace Nodes {
        class Node {
            public:
                void Execute(); // Això per execució un cop construida la estructura del codi
        };

        class Expression {
            public:
                void Evaluate(); // Aquesta funció evalua. S'hauria de cridar al executar-la suposo
        };

        class Literal : Expression {
            public:
                enum LiteralType { // Hauria de tenir suport per llistes o algo
                    INT,
                    REAL,
                    STRING,
                    BOOLEAN
                };
                Literal(std::string value, LiteralType type);
            private:
                std::string value;
                LiteralType type;
        };

        class Binary : Expression {
            public:
                Binary(Expression first, Token::TokenType operation, Expression second);
            private:
                Expression first;
                Expression second;
                Token::TokenType operation;
        };

        class Unary : Expression {
            public:
                Unary(Expression expression, Token::TokenType operation);
            private:
                Expression expression;
                Token::TokenType operation;
        };

        class Call : Expression {
            public:
                Call(std::vector<Expression> args, std::string function);
            private:
                std::vector<Expression> args;
                std::string function;
        };

        class Declaration : Node {
            public:
                Declaration(Token::TokenType variableType, std::string varName);
            private:
                Token::TokenType variableType;
                std::string varName;
        };

        class Assignation : Node {
            public:
                Assignation(std::string varName, Expression expression);
            private:
                std::string varName;
                Expression expression;
        };
    
        class If : Node {
            public:
                If(Expression condition, std::vector<Node> ifChildren, std::vector<Node> elseChildren);
            private:
                Expression condition;
                std::vector<Node> ifChildren;
                std::vector<Node> elseChildren;
        };

        class While : Node {
            public:
                While(Expression condition, std::vector<Node> children);
            private:
                Expression condition;
                std::vector<Node> children;
        };

        class Function : Node {
            public:
                Function(std::string name, std::vector<Declaration> arguments, std::vector<Node> children, Literal::LiteralType returnType);
            private:
                std::string name;
                std::vector<Declaration> arguments;
                std::vector<Node> children;
                Literal::LiteralType returnType;
        };
    }
}
 