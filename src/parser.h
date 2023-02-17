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
            
            void GetExpression(Nodes::Node** parent, int from, int to);
            void Eat(Token tok, Token comp, int *from);
            int EatParentesis(int from);
            void GetArguments(int from, int to, std::vector<Nodes::Node *>* parent);
            bool DigestName(Token::TokenType expected, Token *dir, int *index);

            bool IsType(Token::TokenType type);
    };
}