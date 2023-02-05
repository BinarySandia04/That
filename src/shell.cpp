#include "shell.h"
#include "kernel.h"

#include <iostream>

Radic::Shell::Shell(){

}

Radic::Shell::~Shell(){

}

void Radic::Shell::CreateShell(Kernel *kernel){
    std::string line;

    std::cout << ">> ";

    while(getline(std::cin, line)){

        kernel->compile(line);

        std::cout << ">> ";
    }
}