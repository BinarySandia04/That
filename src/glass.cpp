#include <iostream>
#include "version.h"
#include "shell.h"
#include "kernel.h"

int cli();
int script();

int main(int argc, char *argv[]){

    if(argc < 2){
        printVersion();
        cli();
    } else {
        script();
    }

    return 0;
}

int cli(){
    // Modo cli
    GKernel kernel;
    GShell::CreateShell(&kernel);
}

int script(){

}