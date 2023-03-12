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
            Debug::LogOutput((p+i)->num);
            break;
        case Type::STRING:
            r = (p+i)->num;
            for(int j = 0; j < r; j++){
                Debug::LogOutput((char) ((p+i)->data[j]));
            }
            break;
        case Type::BOOL:
            if(p->num == 1) Debug::LogOutput("True");
            else Debug::LogOutput("False");
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

    ops->insert({{Operator::OP_MOD, Type::INT, Type::INT}, Operations::IntIntMod});

    ops->insert({{Operator::OP_LT, Type::INT, Type::INT}, Operations::IntIntLt});

    ops->insert({{Operator::OP_LEQ, Type::INT, Type::INT}, Operations::IntIntLeq});

    ops->insert({{Operator::OP_GT, Type::INT, Type::INT}, Operations::IntIntGt});
    
    ops->insert({{Operator::OP_GEQ, Type::INT, Type::INT}, Operations::IntIntGeq});

    ops->insert({{Operator::OP_EQ, Type::INT, Type::INT}, Operations::IntIntEq});
    
    ops->insert({{Operator::OP_NEQ, Type::INT, Type::INT}, Operations::IntIntNeq});

    ops->insert({{Operator::OP_AND, Type::BOOL, Type::BOOL}, Operations::BoolBoolAnd});
}
