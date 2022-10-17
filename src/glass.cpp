#include <iostream>
#include "version.h"

// Main loop. S'hauria de comprovar si veure el interpret o no
int main(int argc, char *argv[]){
    printVersion();

    if(argc < 2){
        std::cout << "No has passat res" << std::endl;
    } else {
        std::cout << "Has passat alguna cosa" << std::endl;
    }

    return 0;
}