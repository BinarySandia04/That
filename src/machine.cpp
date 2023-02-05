#include "machine.h"

#include <iostream>

void DebugMem(uint64_t raw[], int n){
    std::cout << "Dump:" << std::endl;
    for(int i = 0; i < n; i++){
        std::cout << raw[i] << " ";
    }
    std::cout << std::endl;
}

Radic::VM::VM(char filename[]){

    FILE* f = fopen(filename, "r");
    
    uint32_t nConst; // 32 bit
    std::cout << sizeof(uint64_t) << std::endl;
    fread(&nConst, sizeof(unsigned int), 1, f);
    // Read constants
    
    std::cout << "Hemos leido " << nConst << " constantes" << std::endl;
    char ins[4];
    uint64_t cons[nConst];

    fread(cons, sizeof(uint64_t), nConst, f);

    DebugMem(cons, nConst);

    while(fread(ins, sizeof(char), 4, f)){
        Process(ins, cons);
    }

    std::cout << std::endl;
}

int Radic::VM::Process(char ins[], uint64_t cons[]){
    int8_t instId = ((int8_t) ins[0] >> 2);
    VM::Instruction tipus = static_cast<VM::Instruction>(instId);
    std::cout << "Hola instrucción! " << (int) instId << " " << tipus << std::endl;

    int8_t b, c;
    uint8_t a;
    unsigned int bx;

    a = this->ReadA(ins);

    switch (tipus)
    {
    case Instruction::LOADC:
        bx = this->ReadBx(ins);
        regs[a] = cons[bx];
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

uint8_t Radic::VM::ReadA(char ins[]){
    return ((uint8_t) ins[0] << 6 | (uint8_t) ins[1] >> 2);
}

uint8_t Radic::VM::ReadB(char ins[]){
    return ((uint8_t) ins[0] << 6 | (uint8_t) ins[1] >> 2);
}

uint8_t Radic::VM::ReadC(char ins[]){
    return ((uint8_t) ins[0] << 6 | (uint8_t) ins[1] >> 2);
}

unsigned int Radic::VM::ReadBx(char ins[]){
    return ((unsigned int) ((char) (ins[1] << 6)) << 10) | (unsigned int) ((unsigned char) ins[2]) << 8 | (unsigned int) ((unsigned char) ins[3]);
}

Radic::VM::~VM(){
    std::cout << "Destroyed vm" << std::endl;
}