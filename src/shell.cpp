#include "shell.h"
#include "kernel.h"

#include <iostream>

Glass::Shell::Shell(){

}

Glass::Shell::~Shell(){

}

void Glass::Shell::CreateShell(Kernel *kernel){
    std::string line;

    std::cout << ">> ";

    while(getline(std::cin, line)){

        kernel->send(line);

        std::cout << ">> ";
    }
}