#include "data.h"

using namespace That;

Instruction::Instruction(){
    Instruction(InstructionID::HALT, ParamType::E);
}

Instruction::Instruction(InstructionID type, ParamType paramType){
    this->type = type;
    this->paramType = paramType;
    
    this->a = INT32_MIN;
    this->b = INT32_MIN;
    this->c = INT32_MIN;
    this->temp = 0;
}

void Instruction::SetA(int a){
    this->a = a;
}
void Instruction::SetB(int b){
    this->b = b;
}
void Instruction::SetC(int c){
    this->c = c;
}

int Instruction::GetA(){
    return this->a;
}
int Instruction::GetB(){
    return this->b;
}
int Instruction::GetC(){
    return this->c;
}