#include "operations.h"
#include <iostream>

// Fitxer doncs ple de operacions i tal
using namespace That;

// +
reg_t Operations::IntIntSum(reg_t* a, reg_t* b){
    reg_t res;
    
    /*
    if(a->num > 0 && b->num > INT32_MAX - a->num){
        res.type = Type::NUMBER;
        // I doncs fer nombre gran
    }
    */

    res.num = a->num + b->num;
    res.type = Type::INT;
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
    // a -> int, b -> str

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
reg_t Operations::StrIntSum(reg_t* a, reg_t *b){
    reg_t res;
    // a -> str, b -> int

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

// *
reg_t Operations::IntIntMul(reg_t *a, reg_t *b){
    reg_t res;
    res.type = Type::INT;
    res.num = a->num * b->num;
    return res;
}

reg_t Operations::IntStrMul(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::STRING;
    res.num = b->num * a->num;

    res.data = new uint8_t[res.num];
    for(int i = 0; i < a->num; i++){
        for(int j = 0; j < b->num; j++){
            res.data[i * b->num + j] = b->data[j];
        }
    }
    return res;
}

reg_t Operations::StrIntMul(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::STRING;
    res.num = b->num * a->num;

    res.data = new uint8_t[res.num];
    for(int i = 0; i < b->num; i++){
        for(int j = 0; j < a->num; j++){
            res.data[i * a->num + j] = a->data[j];
        }
    }
    return res;
}

// -
reg_t Operations::IntIntSub(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::INT;
    res.num = a->num - b->num;
    return res;
}

// /
reg_t Operations::IntIntDiv(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::INT;
    res.num = a->num / b->num;
    return res;
}

// %
reg_t Operations::IntIntMod(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::INT;
    res.num = a->num % b->num;
    return res;
}

// <
reg_t Operations::IntIntLt(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::BOOL;
    res.num = a->num < b->num;
    return res;
}

// >
reg_t Operations::IntIntGt(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::BOOL;
    res.num = a->num > b->num;
    return res;
}

// <=
reg_t Operations::IntIntLeq(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::BOOL;
    res.num = a->num <= b->num;
    return res;
}

// >=
reg_t Operations::IntIntGeq(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::BOOL;
    res.num = a->num >= b->num;
    return res;
}

// ==
reg_t Operations::IntIntEq(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::BOOL;
    res.num = a->num == b->num;
    return res;
}

// !=
reg_t Operations::IntIntNeq(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::BOOL;
    res.num = a->num != b->num;
    return res;
}

// &&
reg_t Operations::BoolBoolAnd(reg_t* a, reg_t* b){
    reg_t res;
    res.type = Type::BOOL;
    res.num = a->num && b->num;
    return res;
}
