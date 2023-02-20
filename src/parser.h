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
            
            void GetCodeFunction(Nodes::Node *root, int from);
            void GetCodeLine(Nodes::Node *root, int from, int to);

            void GetExpression(int from, int to, Nodes::Node** writeNode);
            void Eat(Token tok, Token comp, int *from);

            int GetNext(int from, int lim, Token::TokenType type);

            void GetArguments(int from, int to, std::vector<Nodes::Node *>* parent);

            void GetAssignation(int from, int to, Nodes::Node** writeNode);
            void GetAssignations(int from, int to, std::vector<Nodes::Node *> *container);

            bool DigestName(Token::TokenType expected, Token *dir, int *index);

            bool IsType(Token::TokenType type);
    };
}