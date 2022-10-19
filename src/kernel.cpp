#include <iostream>

#include "kernel.h"
#include "lexer.h"

Glass::Kernel::Kernel() {
    /* Constructor */
}

Glass::Kernel::~Kernel() {
    /* Destructor */
}

void Glass::Kernel::send(std::string code){
    Glass::Lexer lexer(code);
    lexer.GenerateTokens();
    std::vector<Glass::Token> tokens = *(lexer.GetTokens());

    for(int i = 0; i < tokens.size(); i++){
        std::cout << "[";
        std::cout << "type: " << tokens[i].type;
        if(tokens[i].value.size() > 0){
            std::cout << ", value: " << tokens[i].value << "]";
        } else {
            std::cout << "]";
        }
        if(i < tokens.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}