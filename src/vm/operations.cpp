#include "operations.h"
#include <iostream>

// Fitxer doncs ple de operacions i tal
using namespace That;

reg_t Operations::IntIntSum(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::INT;
    res.num = a->num + b->num;
    return res;
}
reg_t Operations::StrStrSum(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::STRING;
    res.num = a->num + b->num;
    res.data = new uint8_t[res.num];
    for(int i = 0; i < a->num; i++){
        res.data[i] = a->data[i];
    }
    for(int i = 0; i < b->num; i++){
        res.data[i + a->num] = b->data[i];
    }
    return res;
}

reg_t Operations::IntStrSum(reg_t* a, reg_t *b){
    reg_t res;

    res.type = Type::STRING;
    std::string num = std::to_string(b->num);
    res.num = a->num + num.size();

    res.data = new uint8_t[res.num];
    for(int i = 0; i < a->num; i++){
        res.data[i] = a->data[i];
    }
    for(int i = 0; i < num.size(); i++){
        res.data[i + a->num] = num[i];
    }

    return res;
}

reg_t Operations::StrIntSum(reg_t* a, reg_t *b){
    reg_t res;

    res.type = Type::STRING;
    std::string num = std::to_string(a->num);
    res.num = b->num + num.size();

    res.data = new uint8_t[res.num];

    for(int i = 0; i < num.size(); i++){
        res.data[i] = num[i];
    }
    for(int i = 0; i < b->num; i++){
        res.data[i + num.size()] = b->data[i];
    }

    return res;
}