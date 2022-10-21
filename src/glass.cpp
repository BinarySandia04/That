#include <iostream>
#include "version.h"
#include "shell.h"
#include "kernel.h"

int cli();
int script(char name[]);

int main(int argc, char *argv[]){

    if(argc < 2){
        printVersion();
        cli();
    } else {
        script(argv[1]);
    }

    return 0;
}

int cli(){
    Glass::Kernel kernel;
    Glass::Shell::CreateShell(&kernel);

    return 0;
}

int script(char name[]){
    Glass::Kernel kernel;

    kernel.sendScript(name);

    return 0;
}