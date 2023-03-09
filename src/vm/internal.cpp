#include "internal.h"
#include "../headers/debug.hpp"
#include "operations.h"

#include <iostream>
#include <map>
#include <tuple>

using namespace That;

#define OP_NUM 4

reg_t Internal::print(reg_t* p, int size){

    int r;
    for(int i = 0; i < size; i++){

        switch((p+i)->type){
        case Type::INT:
            std::cout << (p+i)->num;
            break;
        case Type::STRING:
            r = (p+i)->num;
            for(int j = 0; j < r; j++){
                std::cout << (char) ((p+i)->data[j]);
            }
            break;
        case Type::BOOL:
            if(p->num == 1) std::cout << "True";
            else std::cout << "False";
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

// 
void Internal::LoadOperations(std::map<std::tuple<Operator, Type, Type>, reg_t (*)(reg_t*, reg_t*)> *ops){
    ops->insert({{Operator::OP_ADD, Type::INT, Type::INT}, Operations::IntIntSum});
    ops->insert({{Operator::OP_ADD, Type::STRING, Type::STRING}, Operations::StrStrSum});
    ops->insert({{Operator::OP_ADD, Type::INT, Type::STRING}, Operations::IntStrSum});
    ops->insert({{Operator::OP_ADD, Type::STRING, Type::INT}, Operations::StrIntSum});

    ops->insert({{Operator::OP_MUL, Type::INT, Type::INT}, Operations::IntIntMul});
    ops->insert({{Operator::OP_MUL, Type::STRING, Type::INT}, Operations::StrIntMul});
    ops->insert({{Operator::OP_MUL, Type::INT, Type::STRING}, Operations::IntStrMul});

    ops->insert({{Operator::OP_SUB, Type::INT, Type::INT}, Operations::IntIntSub});

    ops->insert({{Operator::OP_DIV, Type::INT, Type::INT}, Operations::IntIntDiv});

    ops->insert({{Operator::OP_LT, Type::INT, Type::INT}, Operations::IntIntLt});

    ops->insert({{Operator::OP_GT, Type::INT, Type::INT}, Operations::IntIntGt});
}
