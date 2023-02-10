#include "machine.h"

#include <iostream>


void That::VM::MemDump(uint8_t *data, int size){
    std::cout << "[Dump] Size: " << size << std::endl;

    for(int i = 0; i < size; i++){
        std::cout << (int) *(data + i) << std::endl;
    }
    std::cout << "Si" << std::endl;
    return;
}

That::VM::VM(char filename[]){

    FILE* f = fopen(filename, "r");
    
    uint32_t nConst; // 32 bit
    fread(&nConst, sizeof(unsigned int), 1, f);
    // Read constants

    std::cout << "Ok tenim " <<  nConst << " constants" << std::endl;
    uint8_t ins[4];

    uint8_t* cons;
    reg_t* regCons[nConst];

    for(int i = 0; i < nConst; i++){
        // Llegim un uint32_t de size
        reg_t* nReg = new reg_t;
        uint32_t size, typeID; // Es pot cambiar això de tamany?
        fread(&typeID, sizeof(uint32_t), 1, f);

        reg_t::type_t tipus = static_cast<reg_t::type_t>(typeID);
        
        std::cout << "Es de tipus " <<  typeID << std::endl;

        if(tipus == reg_t::INT){
            fread(&(nReg->num), sizeof(nReg->num), 1, f);
        } else {
            fread(&size, sizeof(uint32_t), 1, f);
            std::cout << "Te size " << size << std::endl;
            nReg->num = size;
            
            nReg->data = new uint8_t[size];
            fread(nReg->data, sizeof(uint8_t), size, f);
            MemDump(nReg->data, size);
        }
        
        nReg->type = tipus;
        regCons[i] = nReg;
        
    }

    while(fread(ins, sizeof(uint8_t), 4, f)){
        std::cout << "Leido hasiendo dump" << std::endl;
        MemDump(ins, 4);

        Process(ins, regCons, 0);
    }
}

int That::VM::Process(uint8_t ins[], reg_t* regCons[], int offset){
    int8_t instId = ((int8_t) ins[0]);
    std::cout << "instId: " << (unsigned int) instId << std::endl;
    VM::Instruction tipus = static_cast<VM::Instruction>(instId);

    int8_t b, c;
    uint8_t a;
    uint32_t bx, abx;

    switch (tipus)
    {
    case Instruction::PUSH: // Push reg
        abx = this->ReadAbx(ins);
        std::cout << "Instruccion push " << abx << std::endl;
        if(abx == 0){
            // Push the none
            reg_t* none = new reg_t;
            none->type = reg_t::type_t::NONE;
            regs.push_back(none);
        } else{
            std::cout << "Cola " << (uint32_t) abx << std::endl;
            regs.push_back(regCons[abx]);
        }
        break;
    case Instruction::MOVE: // Move regs
        a = this->ReadA(ins) + offset;
        b = this->ReadB(ins) + offset;
        regs[b] = regs[a];
        break;
    case Instruction::CALL: // User call
        std::cout << "Vale caleado esta" << std::endl;

        a = this->ReadA(ins) + offset;
        b = this->ReadB(ins) + offset;
        c = this->ReadC(ins) + offset;

        Call(a, b, c);
        break;
    case Instruction::ICL: // Internal call
        a = this->ReadA(ins) + offset;
        b = this->ReadB(ins) + offset;
        c = this->ReadC(ins) + offset;

        // Aqui funcions internes o algo
        break;
    default:
        break;
    }
}

void That::VM::Call(uint8_t a, uint8_t b, uint8_t c){
    int n = regs[a]->num;

    uint8_t *data = regs[a]->data;

    for(int i = 0; i < c - b; i++){
        regs.push_back(regs[i]);
    }

    void* pdata = &data;
    uint32_t* cNums = reinterpret_cast<uint32_t*>(pdata);
    uint32_t cNum = cNums[0];

    reg_t* regCons[cNum];

    int j = 0;
    for(int i = 0; i < cNum; i++){
        // Llegim un uint32_t de size
        reg_t* nReg = new reg_t;
        uint32_t size, typeID; // Es pot cambiar això de tamany?
        
        uint32_t* vData = reinterpret_cast<uint32_t*>((void*) (data + j + UINT32_SIZE));
        typeID = *(vData);

        reg_t::type_t tipus = static_cast<reg_t::type_t>(typeID);

        if(tipus == reg_t::INT){
            nReg->num = *(vData + 1);
        } else {
            nReg->num = *(vData + 1);
            nReg->data = data + 2 * UINT32_SIZE;
        }
        
        nReg->type = tipus;
        regCons[i] = nReg;
    }

    int s = regs.size();

    for(int i = 0; i < n - j; i++){
        Process((regs[a]->data) + j + i, regCons, s);
    }

    // Eliminar vars
    for(int i = 0; i < c - b; i++){
        regs.pop_back();
    }

}

uint8_t That::VM::ReadA(uint8_t ins[]){
    return ((uint8_t) ins[1]);
}

uint8_t That::VM::ReadB(uint8_t ins[]){
    return ((uint8_t) ins[2]);
}

uint8_t That::VM::ReadC(uint8_t ins[]){
    return ((uint8_t) ins[3]);
}

uint32_t That::VM::ReadBx(uint8_t ins[]){
    return ((uint32_t) ((uint8_t) (ins[2]) << 8) | (uint32_t) ((uint8_t) ins[3]));
}

uint32_t That::VM::ReadAbx(uint8_t ins[]){
    return ((uint32_t) ((uint8_t) (ins[1])) | (uint32_t) ((uint8_t) (ins[2]) << 8) | (uint32_t) ((uint8_t) (ins[3]) << 16));
}

That::VM::~VM(){
    std::cout << "Destroyed vm" << std::endl;
}