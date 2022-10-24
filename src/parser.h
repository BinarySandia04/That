#pragma once

#include <vector>

#include "lexer.h"
#include "nodes.h"

namespace Glass {

    class Parser {
        public:
            Parser(std::vector<Glass::Token> tokens);

            Nodes::Node GenerateAST();
        private:
            std::vector<Glass::Token> tokens;
    };
}