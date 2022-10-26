#include "parser.h"

#include <vector>

using namespace Glass;

Parser::Parser(std::vector<Token> tokens){
    this->tokens = tokens;
}

Nodes::Node Parser::GenerateAST(){
    Nodes::Node root;

    int next = 0;
    while(next < tokens.size()){
        GetExpression(&root);
    }
}