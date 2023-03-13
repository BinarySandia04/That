#include "operations.h"
#include <iostream>

// Fitxer doncs ple de operacions i tal
using namespace That;

// +
void Operations::IntIntSum(reg_t* a, reg_t* b, reg_t* c){
    c->num = a->num + b->num;
    c->type = Type::INT;
}

void Operations::StrStrSum(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::STRING;
    c->num = a->num + b->num;
    c->data = new uint8_t[c->num];
    for(int i = 0; i < a->num; i++){
        c->data[i] = a->data[i];
    }
    for(int i = 0; i < b->num; i++){
        c->data[i + a->num] = b->data[i];
    }
}
void Operations::IntStrSum(reg_t* a, reg_t *b, reg_t* c){
    // a -> int, b -> str

    c->type = Type::STRING;
    std::string num = std::to_string(a->num);
    c->num = b->num + num.size();

    c->data = new uint8_t[c->num];
    for(int i = 0; i < num.size(); i++){
        c->data[i] = num[i];
    }
    for(int i = 0; i < b->num; i++){
        c->data[i + num.size()] = b->data[i];
    }
}
void Operations::StrIntSum(reg_t* a, reg_t *b, reg_t* c){
    // a -> str, b -> int
    c->type = Type::STRING;
    std::string num = std::to_string(b->num);
    c->num = a->num + num.size();

    c->data = new uint8_t[c->num];

    for(int i = 0; i < a->num; i++){
        c->data[i] = a->data[i];
    }
    for(int i = 0; i < num.size(); i++){
        c->data[i + a->num] = num[i];
    }
}

// *
void Operations::IntIntMul(reg_t *a, reg_t *b, reg_t* c){
    c->type = Type::INT;
    c->num = a->num * b->num;
}

void Operations::IntStrMul(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::STRING;
    c->num = b->num * a->num;

    c->data = new uint8_t[c->num];
    for(int i = 0; i < a->num; i++){
        for(int j = 0; j < b->num; j++){
            c->data[i * b->num + j] = b->data[j];
        }
    }
}

void Operations::StrIntMul(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::STRING;
    c->num = b->num * a->num;

    c->data = new uint8_t[c->num];
    for(int i = 0; i < b->num; i++){
        for(int j = 0; j < a->num; j++){
            c->data[i * a->num + j] = a->data[j];
        }
    }
}

// -
void Operations::IntIntSub(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::INT;
    c->num = a->num - b->num;
}

// /
void Operations::IntIntDiv(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::INT;
    c->num = a->num / b->num;
}

// %
void Operations::IntIntMod(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::INT;
    c->num = a->num % b->num;
}

// <
void Operations::IntIntLt(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::BOOL;
    c->num = a->num < b->num;
}

// >
void Operations::IntIntGt(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::BOOL;
    c->num = a->num > b->num;
}

// <=
void Operations::IntIntLeq(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::BOOL;
    c->num = a->num <= b->num;
}

// >=
void Operations::IntIntGeq(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::BOOL;
    c->num = a->num >= b->num;
}

// ==
void Operations::IntIntEq(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::BOOL;
    c->num = a->num == b->num;
}

// !=
void Operations::IntIntNeq(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::BOOL;
    c->num = a->num != b->num;
}

// &&
void Operations::BoolBoolAnd(reg_t* a, reg_t* b, reg_t* c){
    c->type = Type::BOOL;
    c->num = a->num && b->num;
}
