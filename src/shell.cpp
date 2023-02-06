#include "shell.h"
#include "kernel.h"

#include <iostream>

That::Shell::Shell(){

}

That::Shell::~Shell(){

}

void That::Shell::CreateShell(Kernel *kernel){
    std::string line;

    std::cout << ">> ";

    while(getline(std::cin, line)){

        kernel->compile(line);

        std::cout << ">> ";
    }
}