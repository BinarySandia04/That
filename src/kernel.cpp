#include <iostream>
#include <fstream>
#include <limits>
#include <cstdint>

#include "kernel.h"
#include "lexer.h"
#include "parser.h"
#include "machine.h"

#define BLOCK_SIZE 1024

Radic::Kernel::Kernel() {
    /* Constructor */
}

Radic::Kernel::~Kernel() {
    /* Destructor */
}

void Radic::Kernel::compile(std::string code){
    Radic::Lexer lexer(code);

    lexer.GenerateTokens();
    std::vector<Radic::Token> tokens = *(lexer.GetTokens());


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


    Radic::Parser parser(tokens);

    Radic::Nodes::Node ast = parser.GenerateAST();
}

void Radic::Kernel::send(char filename[]){
    // Initialize vm
    
    VM vm(filename);

    // Ara partim això en coses nose com shorts per exemple
}

void Radic::Kernel::sendScript(char name[]){
    std::fstream file(name);

    std::string code = "", line;
    while(std::getline(file, line)){
        code += line + "\n";
    }
    compile(code);
}