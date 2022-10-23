#pragma once

#include <vector>
#include <string>

#include "lexer.h"

namespace Glass {
    namespace Nodes {
        class Node {
            public:
                Node();
                ~Node();
        };

        class Expression : Node {
            public:
                Expression();
                ~Expression();
                void Evaluate();
        };

        class Literal : Expression {
            enum LiteralType {
                INT,
                REAL,
                STRING,
                BOOLEAN
            };
            public:
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
    }
}
 