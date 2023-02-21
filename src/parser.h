#pragma once

#include <vector>

#include "lexer.h"
#include "nodes.h"

namespace That {

    class Parser {
        public:
            Parser(std::vector<That::Token> tokens);

            Nodes::Node GenerateAST();
        private:
            std::vector<That::Token> tokens;
            
            void GenerateCode(int from, int to, Nodes::Node *parent);

            void GetCodeFunction(Nodes::Node **root, int from, int *end);
            void GetCodeLine(Nodes::Node *root, int from, int to);
            void GetCodeConditional(Nodes::Node **root, int from, int *end);

            void GetExpression(int from, int to, Nodes::Node** writeNode);
            bool Eat(Token::TokenType tok, Token::TokenType comp, int *from);

            int GetNext(int from, int lim, Token::TokenType type);

            void GetArguments(int from, int to, std::vector<Nodes::Node *>* parent);
            void GetFunctionParameters(int from, int to, std::vector<Nodes::Node *>* container);
            void GetFunctionParameter(int from, int to, Nodes::Node** writeNode);
            void GetAssignation(int from, int to, Nodes::Node** writeNode);
            void GetAssignations(int from, int to, std::vector<Nodes::Node *> *container);

            bool IsType(Token::TokenType type);
    };
}