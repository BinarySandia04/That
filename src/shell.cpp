#include "shell.h"
#include "kernel.h"

#include <iostream>
#include "headers/termcolor.hpp"

That::Shell::Shell(){

}

That::Shell::~Shell(){

}

void That::Shell::CreateShell(Kernel *kernel){
    std::string line;

    std::cout << termcolor::color<255, 168, 46> << termcolor::bold << ">> " << termcolor::reset;

    while(getline(std::cin, line)){

        kernel->compile(line);

        std::cout << termcolor::color<255, 168, 46> << termcolor::bold << ">> " << termcolor::reset;
    }
}