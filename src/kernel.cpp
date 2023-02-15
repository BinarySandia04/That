#include <iostream>
#include <fstream>
#include <limits>
#include <cstdint>

#include <gmp.h>

#include "kernel.h"
#include "lexer.h"
#include "parser.h"
#include "vm/machine.h"

That::Kernel::Kernel() {
    /* Constructor */
}

That::Kernel::~Kernel() {
    /* Destructor */
}

void That::Kernel::compile(std::string code){
    That::Lexer lexer(code);

    lexer.GenerateTokens();
    std::vector<That::Token> tokens = *(lexer.GetTokens());


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


    That::Parser parser(tokens);

    That::Nodes::Node ast = parser.GenerateAST();
}

void That::Kernel::send(char filename[]){
    // Initialize vm
    
    VM vm(filename);

    // Ara partim això en coses nose com shorts per exemple
}

void That::Kernel::sendScript(char name[]){
    std::fstream file(name);

    std::string code = "", line;
    while(std::getline(file, line)){
        code += line + "\n";
    }
    compile(code);
}