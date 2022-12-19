#include "shell.h"
#include "kernel.h"

#include <iostream>

Rux::Shell::Shell(){

}

Rux::Shell::~Shell(){

}

void Rux::Shell::CreateShell(Kernel *kernel){
    std::string line;

    std::cout << ">> ";

    while(getline(std::cin, line)){

        kernel->send(line);

        std::cout << ">> ";
    }
}