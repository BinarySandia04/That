#pragma once

#include <vector>
#include <string>

#include "lexer.h"

namespace Radic {
    namespace Nodes {

        enum NodeType {
            NODE,
            DECLARATION,
            ASSIGNATION,
            IF,
            WHILE,
            FUNCTION,
            NODE_ERROR
        };

        enum ExpressionType {
            BINARY,
            UNARY,
            CALL,
            VALUE,
            ERROR,
        };

        class Node {

            public:
                Node(NodeType type);

                void Execute(); // Això per execució un cop construida la estructura del codi
                
                std::vector<Node*> children;
                NodeType GetType();

            private:
                NodeType type;
                Node* parent;
        };

        class Error : Node {
            public:
                Error(int errorCode);
                int GetErrorCode();
            private:
                int errorCode;
        };
        
        class Literal {
            public:
                enum LiteralType { // Hauria de tenir suport per llistes o algo
                    INT,
                    REAL,
                    STRING,
                    BOOLEAN,
                    VOID
                };
                
                Literal(std::string value, LiteralType type);
                Literal();
                std::string GetValue();
                LiteralType GetLiteralType();
            private:
                std::string value;
                LiteralType type;
        };

        class Expression {
            public:
                Expression();
                Expression(ExpressionType type);
                Expression(Literal value);

                ExpressionType GetType();
                Expression *first;
                Literal value;
                virtual Literal Evaluate(); // Aquesta funció evalua. S'hauria de cridar al executar-la suposo
            private:
                ExpressionType expType;
        };

        class ExpressionError : Expression {
            public:
                ExpressionError(int errorCode);
            private:
        };


        class Binary : Expression {
            public:
                Literal Evaluate();
                Binary(Expression *first, Token::TokenType operation, Expression *second);
            private:
                Expression *second;
                Token::TokenType operation;
        };

        class Unary : Expression {
            public:
                Literal Evaluate();
                Unary(Expression *expression, Token::TokenType operation);
            private:
                Token::TokenType operation;
        };

        class Call : Expression {
            public:
                Literal Evaluate();
                Call(std::vector<Expression*> args, std::string function);
            private:
                std::vector<Expression*> args;
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
                Assignation(std::string varName, Expression *expression);
            private:
                std::string varName;
                Expression *expression;
        };
    
        class If : Node {
            public:
                If(Expression *condition, std::vector<Node*> ifChildren, std::vector<Node*> elseChildren);
            private:
                Expression *condition;
                std::vector<Node*> ifChildren;
                std::vector<Node*> elseChildren;
        };

        class While : Node {
            public:
                While(Expression *condition, std::vector<Node*> children);
            private:
                Expression *condition;
        };

        class Function : Node {
            public:
                Function(std::string name, std::vector<Declaration*> arguments, std::vector<Node*> children, Literal::LiteralType returnType);
            private:
                std::string name;
                std::vector<Declaration*> arguments;
                Literal::LiteralType returnType;
        };
    }
}
 