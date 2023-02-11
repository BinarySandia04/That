#include "internal.h"

#include <iostream>

void That::print(That::VM::reg_t* reg){
    switch(reg->type){
        case That::VM::reg_t::INT:
            std::cout << ">> " << reg->num << std::endl;
            break;
        case That::VM::reg_t::STR:
            std::cout << ">> ";
            for(int i = 0; i < reg->num; i++){
                std::cout << (char) reg->data[i];
            }
            std::cout << std::endl;
        default:
            break;
    }
}