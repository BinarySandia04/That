#include "serializer.h"
#include "../headers/debug.hpp"
#include "../vm/data.h"

using namespace That;

void Serializer::Serialize(MachineCode assembly){

}

void Serializer::SerializeToFile(std::string fileName, MachineCode assembly){
    // Val i ara d'alguna manera doncs hem de parsear això. Anem a posar tot el tema de constants
    FILE* f = fopen(fileName.c_str(), "wb");
    if(!f){
        Debug::LogError("Error writing to file!");
        return;
    }

    unsigned int regCount = assembly.regCount;
    fwrite(&regCount, sizeof(unsigned int), 1, f);

    unsigned int s = assembly.constants.size();
    fwrite(&s, sizeof(unsigned int), 1, f);

    for(unsigned int i = 0; i < s; i++){
        WriteConst(f, assembly.constants[i].data);
    }

    // Val i ara doncs venen les instruccions
    for(int i = 0; i < assembly.instructions.size(); i++){
        WriteInstruction(f, assembly.instructions[i]);
    }

    fclose(f);
}

void Serializer::SerializeFromFile(std::string fileName, MachineCode *code){
    FILE *f = fopen(fileName.c_str(), "rb");
    if(!f){
        Debug::LogError("Error reading file " + fileName + "!");
        return;
    }  
    
    MachineCode machinCode;

    unsigned int regCount;
    fread(&regCount, sizeof(unsigned int), 1, f);

    machinCode.regCount = regCount;

    unsigned int s; // Constant size
    fread(&s, sizeof(unsigned int), 1, f);

    std::cout << "S: " << s << std::endl;

    for(unsigned int i = 0; i < s; i++){
        ReadConst(f, &machinCode.constants);
    }

    while(ReadInstruction(f, &machinCode.instructions)); // Lee instrucciones hasta que se acaben creo

    fclose(f);
    *code = machinCode;
}

void Serializer::WriteInstruction(FILE *f, Instruction ins){
    // Val de moment posarem les tres coses pero caldrà fer bit shifting segons el tamany de les instruccions
    uint8_t i;
    int16_t a,b,c;
    i = (uint8_t) ins.type;
    a = (int16_t) ins.GetA();
    b = (int16_t) ins.GetB();
    c = (int16_t) ins.GetC();

    fwrite(&i, sizeof(uint8_t), 1, f);
    fwrite(&a, sizeof(int16_t), 1, f);
    fwrite(&b, sizeof(int16_t), 1, f);
    fwrite(&c, sizeof(int16_t), 1, f);
}

void Serializer::WriteConst(FILE* f, reg_t reg){
    unsigned int size;

    unsigned int type = (unsigned int) reg.type;
    fwrite(&type, sizeof(unsigned int), 1, f);

    switch(reg.type){
    case reg_t::INT:
        size = 1;
        fwrite(&reg.num, sizeof(int), 1, f);
        break;
    case reg_t::NUMBER:
        size = reg.num;
        fwrite(&size, sizeof(unsigned int), 1, f);
        fwrite(reg.data, sizeof(uint8_t), size, f);
        break;
    case reg_t::BOOLEAN:
        size = 1;
        fwrite(&reg.num, sizeof(int), 1, f);
        break;
    case reg_t::REAL:
        size = reg.num;
        fwrite(&size, sizeof(unsigned int), 1, f);
        fwrite(reg.data, sizeof(uint8_t), size, f);
        break;
    case reg_t::STRING:
        size = reg.num;
        fwrite(&size, sizeof(unsigned int), 1, f);
        fwrite(reg.data, sizeof(uint8_t), size, f);
        break;
    default:
        size = 0;
        break;
    }
}

void Serializer::ReadConst(FILE *f, std::vector<Constant> *constants){
    // Llegim el tipus!
    unsigned int typeNum;
    fread(&typeNum, sizeof(unsigned int), 1, f);

    reg_t::type_t type = (reg_t::type_t) typeNum;

    Constant c;
    unsigned int size;

    switch (type)
    {
    case reg_t::INT:
        fread(&c.data.num, sizeof(int), 1, f);
        break;
    case reg_t::NUMBER:
        fread(&size, sizeof(unsigned int), 1, f);
        fread(&c.data.data, sizeof(uint8_t), size, f);
        break;
    case reg_t::BOOLEAN:
        fread(&c.data.num, sizeof(int), 1, f);
        break;
    case reg_t::REAL:
        fread(&size, sizeof(unsigned int), 1, f);
        fread(&c.data.data, sizeof(uint8_t), size, f);
        break;
    case reg_t::STRING:
        fread(&size, sizeof(unsigned int), 1, f);
        fread(&c.data.data, sizeof(uint8_t), size, f);
        break;
    default:
        break;
    }

    c.data.type = type;
    constants->push_back(c);
}

bool Serializer::ReadInstruction(FILE *f, std::vector<Instruction> *instructions){
    // Val de moment posarem les tres coses pero caldrà fer bit shifting segons el tamany de les instruccions
    Instruction ins;

    uint8_t i;
    int16_t a,b,c;

    if(!fread(&i, sizeof(uint8_t), 1, f)) return false;
    if(!fread(&a, sizeof(int16_t), 1, f)) return false;
    if(!fread(&b, sizeof(int16_t), 1, f)) return false;
    if(!fread(&c, sizeof(int16_t), 1, f)) return false;

    ins.type = (InstructionID) i;
    ins.SetA(a);
    ins.SetB(b);
    ins.SetC(c);

    instructions->push_back(ins);

    return true;
}