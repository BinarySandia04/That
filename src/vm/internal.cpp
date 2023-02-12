#include "internal.h"
#include "../headers/debug.hpp"

#include <iostream>

That::reg_t That::Internal::print(That::reg_t* a, That::reg_t* b){
    while(a <= b){
        switch(a->type){
            case That::reg_t::INT:
                Debug::Print(a->num);
                std::cout << std::endl;
                break;
            case That::reg_t::STR:
                for(int i = 0; i < a->num; i++){
                    Debug::Print(a->data[i]);
                }
                std::cout << std::endl;
            default:
                break;
        }
        a++;
    }

}
