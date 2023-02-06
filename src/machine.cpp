#include "machine.h"

#include <iostream>

That::VM::VM(char filename[]){

    FILE* f = fopen(filename, "r");
    
    uint32_t nConst; // 32 bit
    std::cout << sizeof(uint64_t) << std::endl;
    fread(&nConst, sizeof(unsigned int), 1, f);
    // Read constants
    
    std::cout << "Hemos leido " << nConst << " constantes" << std::endl;
    char ins[4];

    uint8_t* cons;
    reg_t regCons[nConst];

    for(int i = 0; i < nConst; i++){
        // Llegim un uint32_t de size
        reg_t nReg;
        uint32_t size, typeID; // Es pot cambiar això de tamany?
        fread(&typeID, sizeof(uint32_t), 1, f);
        fread(&size, sizeof(uint32_t), 1, f);

        std::cout << "La size es " << size << std::endl;
        std::cout << "El tipus es " << typeID << std::endl;

        if(size == 0){
            nReg.type = nReg.INT;
            fread(&nReg.num, sizeof(nReg.num), 1, f);
            continue;
        }
        
        nReg.type = static_cast<reg_t::type_t>(typeID);
        
        fread(&nReg.lnum, sizeof(uint8_t), size, f);
    }

    while(fread(ins, sizeof(char), 4, f)){
        Process(ins, regCons);
    }

    std::cout << std::endl;
}

int That::VM::Process(char ins[], reg_t regCons[]){
    int8_t instId = ((int8_t) ins[0] >> 2);
    VM::Instruction tipus = static_cast<VM::Instruction>(instId);
    std::cout << "Hola instrucción! " << (int) instId << " " << tipus << std::endl;

    int8_t b, c;
    uint8_t a;
    uint32_t bx;

    a = this->ReadA(ins);

    switch (tipus)
    {
    case Instruction::LOADC:
        bx = this->ReadBx(ins);
        regs[a] = regCons[bx];
        break;
    case Instruction::MOVE:
        b = this->ReadB(ins);
        c = this->ReadC(ins);
        regs[c] = regs[b];
        break;
    default:
        break;
    }
}

uint8_t That::VM::ReadA(char ins[]){
    return ((uint8_t) ins[0] << 6 | (uint8_t) ins[1] >> 2);
}

uint8_t That::VM::ReadB(char ins[]){
    return ((uint8_t) ins[0] << 6 | (uint8_t) ins[1] >> 2);
}

uint8_t That::VM::ReadC(char ins[]){
    return ((uint8_t) ins[0] << 6 | (uint8_t) ins[1] >> 2);
}

uint32_t That::VM::ReadBx(char ins[]){
    return ((unsigned int) ((char) (ins[1] << 6)) << 10) | (unsigned int) ((unsigned char) ins[2]) << 8 | (unsigned int) ((unsigned char) ins[3]);
}

That::VM::~VM(){
    std::cout << "Destroyed vm" << std::endl;
}