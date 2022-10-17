#include <iostream>
#include "version.h"

// Main loop. S'hauria de comprovar si veure el interpret o no
int main(){
    printVersion();

    if((std::cin.rdbuf() -> in_avail() > 0)){
        std::cout << "Has passat alguna cosa" << std::endl;
    } else {
        std::cout << "No has passat res" << std::endl;
    }

    return 0;
}