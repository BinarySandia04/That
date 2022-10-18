#include "shell.h"
#include "kernel.h"

#include <iostream>

GShell::GShell(){

}

GShell::~GShell(){

}

GShell GShell::CreateShell(GKernel *kernel){
    std::string line;

    std::cout << ">> ";
    while(getline(std::cin, line)){

        std::cout << line << std::endl;

        std::cout << ">> ";
    }
}