#include "machine.h"

#include <iostream>
#include "internal.h"

void That::VM::MemDump(uint8_t *data, int size){
    std::cout << "[Dump] Size: " << size << std::endl;

    for(int i = 0; i < size; i++){
        std::cout << (int) *(data + i) << " ";
    }
    std::cout << std::endl;
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

    std::cout << "Ok ja hem llegit consts" << std::endl;

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
        
        std::cout << "Cola " << (uint32_t) abx << std::endl;
        regs.push_back(regCons[abx]);
        
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
        std::cout << "Internal call!" << std::endl;
        a = this->ReadA(ins);
        b = this->ReadB(ins) + offset;
        c = this->ReadC(ins) + offset;

        std::cout << (int) b << std::endl;
        std::cout << regs.size() << std::endl;

        // Aqui funcions internes o algo
        if(a == 0){ // Print
            print(regs[b]->num);
        }
        break;
    default:
        break;
    }
}

void That::VM::Call(uint8_t a, uint8_t b, uint8_t c){
    int n = regs[a]->num;

    uint8_t *data = regs[a]->data;

    std::cout << "Data de la función " << (int) a << " calleada:" << std::endl;
    MemDump(regs[a]->data, n);

    for(int i = 0; i < c - b; i++){
        regs.push_back(regs[i]);
    }

    uint32_t cNum = ReadBytes(data); // 0 - 4

    std::cout << "hola " << (unsigned int) cNum << std::endl;
    reg_t* regCons[cNum];

    int j = UINT32_SIZE; // 4
    for(int i = 0; i < cNum; i++){
        // Llegim un uint32_t de size
        reg_t* nReg = new reg_t;
        uint32_t size, typeID; // Es pot cambiar això de tamany?
        
        // uint32_t* vData = reinterpret_cast<uint32_t*>((void*) (data + j + UINT32_SIZE));
        typeID = ReadBytes(data + j);
        j += 4;

        reg_t::type_t tipus = static_cast<reg_t::type_t>(typeID);
        
        nReg->num = ReadBytes(data + j);
        std::cout << "El num es " << nReg->num << std::endl;
        j += 4;
        if(tipus != reg_t::INT){
            nReg->data = data + j;
            j += nReg->num;
        }
        
        nReg->type = tipus;
        regCons[i] = nReg;
    }

    int s = regs.size();

    for(int i = 0; i < n - j; i += 4){
        MemDump((regs[a]->data) + j + i, 4);
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
    return ((uint32_t) (ins[2] << 8) | (uint32_t) ins[3]);
}

uint32_t That::VM::ReadAbx(uint8_t ins[]){
    return ((uint32_t) (ins[1]) | (uint32_t) (ins[2] << 8) | (uint32_t) (ins[3] << 16));
}

uint32_t That::VM::ReadBytes(uint8_t *dir){
    return ((uint32_t) *(dir) | (uint32_t) (*(dir + 1) << 8) | (uint32_t) (*(dir + 2) << 16) | (uint32_t) (*(dir + 3) << 24));
}

That::VM::~VM(){
    std::cout << "Destroyed vm" << std::endl;
}