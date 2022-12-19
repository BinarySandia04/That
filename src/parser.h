#pragma once

#include <vector>

#include "lexer.h"
#include "nodes.h"

namespace Rux {

    class Parser {
        public:
            Parser(std::vector<Rux::Token> tokens);

            Nodes::Node GenerateAST();
        private:
            std::vector<Rux::Token> tokens;
            
            void GetExpression(Nodes::Expression** parent, int from, int to);
    };
}