#pragma once

#include <vector>

#include "lexer.h"
#include "nodes.h"

namespace Radic {

    class Parser {
        public:
            Parser(std::vector<Radic::Token> tokens);

            Nodes::Node GenerateAST();
        private:
            std::vector<Radic::Token> tokens;
            
            void GetExpression(Nodes::Expression** parent, int from, int to);
    };
}