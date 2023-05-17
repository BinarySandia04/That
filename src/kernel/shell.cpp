#include "shell.h"
#include "kernel.h"

#include <iostream>
#include "../headers/termcolor.hpp"
#include "../flags/flags.h"

That::Shell::Shell(){

}

That::Shell::~Shell(){

}

void That::Shell::CreateShell(Kernel *kernel){
    std::cout<<"POV: UR IN A SHELL";
    std::string line;

    std::cout << termcolor::color<255, 168, 46> << termcolor::bold << ">> " << termcolor::reset;

    while(getline(std::cin, line)){

        // kernel->Compile(line, Flag::Flags::RUN);

        std::cout << termcolor::color<255, 168, 46> << termcolor::bold << ">> " << termcolor::reset;
    }
}
