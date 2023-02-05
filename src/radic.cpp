#include <iostream>
#include "version.h"
#include "shell.h"
#include "kernel.h"

int cli();
int script(char name[]);
int byteScript(char name[]);

int main(int argc, char *argv[]){

    if(argc < 2){
        printVersion();
        cli();
    } else {
        byteScript(argv[1]);
    }

    return 0;
}

int cli(){
    Radic::Kernel kernel;
    Radic::Shell::CreateShell(&kernel);

    return 0;
}

int script(char name[]){
    Radic::Kernel kernel;

    kernel.sendScript(name);

    return 0;
}

int byteScript(char name[]){
    Radic::Kernel kernel;

    kernel.send(name);

    return 0;
}