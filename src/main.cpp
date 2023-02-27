#include <iostream>
#include <vector>
#include <string>

#include "kernel/kernel.h"
#include "kernel/shell.h"
#include "headers/debug.hpp"

using namespace That;

int Run(Flag::RunInfo runInfo);

int main(int argc, char *argv[]){

    // Crear vector de args
    std::vector<std::string> args;
    for(int i = 1; i < argc; i++){
        args.push_back(std::string(argv[i]));
    }

    Flag::RunInfo runInfo = Flag::GetRunInfo(args);

    return Run(runInfo);
}

int Run(Flag::RunInfo runInfo){
    That::Kernel kernel;

    if(runInfo.files.size() == 0){
        kernel.PrintVersion();
        kernel.Cli();
    }

    kernel.SendScript(runInfo.files[0], runInfo.flags);
    

    return 0;
}