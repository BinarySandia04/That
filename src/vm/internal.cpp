#include "internal.h"
#include "../headers/debug.hpp"

#include <iostream>

That::reg_t That::Internal::print(That::reg_t* p, int size){

    for(int i = 0; i < size; i++){
        std::cout << (p+i)->num << std::endl;
    }

    return That::reg_t();

}
