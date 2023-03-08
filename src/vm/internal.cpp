#include "internal.h"
#include "../headers/debug.hpp"

#include <iostream>
#include <map>
#include <tuple>

using namespace That;

#define OP_NUM 4

reg_t Internal::print(reg_t* p, int size){

    for(int i = 0; i < size; i++){
        switch((p+i)->type){
        case Type::INT:
            std::cout << (p+i)->num;
            break;
        case Type::STRING:
            int r = (p+i)->num;
            for(int j = 0; j < r; j++){
                std::cout << (char) ((p+i)->data[j]);
            }
            break;
        }
    }

    std::cout << std::endl;

    return reg_t();

}

void Internal::LoadDefaultFunctions(std::vector<reg_t (*)(reg_t*, int)> *func){
    func->push_back(print);
}

// Encara per fer??? Suposo
void Internal::LoadInternalFunctions(std::vector<reg_t (*)(reg_t*, int)> *func){
    
}

void Internal::LoadConversions(std::map<std::tuple<Type, Type>, That::reg_t (*)(That::reg_t*, That::reg_t*)> *conv){
    // conv->insert({{0, 0}, })
}

// Test
reg_t IntSum(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::INT;
    res.num = a->num + b->num;
    return res;
}

// 
void Internal::LoadOperations(std::map<std::tuple<Operator, Type>, reg_t (*)(reg_t*, reg_t*)> *ops){
    ops->insert({{Operator::OP_ADD, Type::INT}, IntSum});
}
