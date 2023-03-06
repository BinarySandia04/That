#include "serializer.h"
#include "../headers/debug.hpp"

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

void Serializer::WriteInstruction(FILE *f, Instruction ins){
    // Val de moment posarem les tres coses pero caldrà fer bit shifting segons el tamany de les instruccions
    uint8_t i;
    int16_t a,b,x;
    i = (uint8_t) ins.type;
    a = (int16_t) ins.a;
    b = (int16_t) ins.b;
    x = (int16_t) ins.x;

    fwrite(&i, sizeof(uint8_t), 1, f);
    fwrite(&a, sizeof(int16_t), 1, f);
    fwrite(&b, sizeof(int16_t), 1, f);
    fwrite(&x, sizeof(int16_t), 1, f);
}

void Serializer::WriteConst(FILE* f, reg_t reg){
    unsigned int size;

    switch(reg.type){
        case reg_t::INT:
            size = 1;
            fwrite(&size, sizeof(int), 1, f);
            fwrite(&reg.num, sizeof(int), 1, f);
            break;
        case reg_t::BOOLEAN:
            size = 1;
            fwrite(&size, sizeof(int), 1, f);
            fwrite(&reg.num, sizeof(int), 1, f);
            break;
        case reg_t::REAL:
            size = reg.num;
            fwrite(&size, sizeof(int), 1, f);
            fwrite(reg.data, sizeof(uint8_t), size, f);
            break;
        case reg_t::STRING:
            size = reg.num;
            fwrite(&size, sizeof(int), 1, f);
            fwrite(reg.data, sizeof(uint8_t), size, f);
            break;
        default:
            size = 0;
            fwrite(&size, sizeof(int), 1, f);
            break;
    }
}