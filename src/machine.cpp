#include "machine.h"

#include <iostream>

That::VM::VM(char filename[]){

    FILE* f = fopen(filename, "r");
    
    uint32_t nConst; // 32 bit
    fread(&nConst, sizeof(unsigned int), 1, f);
    // Read constants
    char ins[4];

    uint8_t* cons;
    reg_t* regCons[nConst];

    for(int i = 0; i < nConst; i++){
        // Llegim un uint32_t de size
        reg_t* nReg = new reg_t;
        uint32_t size, typeID; // Es pot cambiar això de tamany?
        fread(&typeID, sizeof(uint32_t), 1, f);

        if(typeID == 1){
            fread(&(nReg->num), sizeof(nReg->num), 1, f);
        } else {
            fread(&size, sizeof(uint32_t), 1, f);
            
            fread(&(nReg->lnum), sizeof(uint8_t), size, f);
        }
        
        nReg->type = static_cast<reg_t::type_t>(typeID);
        regCons[i] = nReg;
        
    }

    while(fread(ins, sizeof(char), 4, f)){
        Process(ins, regCons);
    }
}

int That::VM::Process(char ins[], reg_t* regCons[]){
    int8_t instId = ((int8_t) ins[0]);
    VM::Instruction tipus = static_cast<VM::Instruction>(instId);

    int8_t b, c;
    uint8_t a;
    uint32_t bx, abx;

    switch (tipus)
    {
    case Instruction::PUSH:
        abx = this->ReadAbx(ins);
        if(abx == 0){
            // Push the none
            reg_t* none = new reg_t;
            none->type = reg_t::type_t::NONE;
            regs.push_back(none);
        } else{
            regs.push_back(regCons[abx-1]);
        }
        break;
    case Instruction::MOVE:
        a = this->ReadA(ins);
        b = this->ReadB(ins);
        c = this->ReadC(ins);
        regs[c] = regs[b];
        break;
    default:
        break;
    }
}

uint8_t That::VM::ReadA(char ins[]){
    return ((uint8_t) ins[1]);
}

uint8_t That::VM::ReadB(char ins[]){
    return ((uint8_t) ins[2]);
}

uint8_t That::VM::ReadC(char ins[]){
    return ((uint8_t) ins[3]);
}

uint32_t That::VM::ReadBx(char ins[]){
    return ((uint32_t) ((uint8_t) (ins[2]) << 8) | (uint32_t) ((uint8_t) ins[3]));
}

uint32_t That::VM::ReadAbx(char ins[]){
    return ((uint32_t) ((uint8_t) (ins[1]) << 16) | (uint32_t) ((uint8_t) (ins[2]) << 8) | (uint32_t) ((uint8_t) ins[3]));
}

That::VM::~VM(){
    std::cout << "Destroyed vm" << std::endl;
}