#include <iostream>
#include <fstream>

#include "kernel.h"
#include "lexer.h"
#include "parser.h"

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

#ifdef DEBUG
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
#endif

    Glass::Parser parser(tokens);

    Glass::Nodes::Node ast = parser.GenerateAST();
}

void Glass::Kernel::sendScript(char name[]){
    std::fstream file(name);

    std::string code = "", line;
    while(std::getline(file, line)){
        code += line + "\n";
    }
    send(code);
}