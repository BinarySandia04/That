#include <iostream>

#include "machine.h"
#include "internal.h"
#include "../headers/debug.hpp"

using namespace That;

std::map<Internal::InternalFunctions, reg_t (*)(reg_t*, int)> VM::FunctionMap = {
    {That::Internal::PRINT, That::Internal::print}
};

void VM::MemDump(uint8_t *data, int size){
    std::cout << "[Dump] Size: " << size << std::endl;

    for(int i = 0; i < size; i++){
        std::cout << (int) *(data + i) << " ";
    }
    std::cout << std::endl;
    return;
}

void VM::Run(MachineCode code){
    currentCode = code;

    // Reservar memoria bonita para todo el code de machinecode
    registers = new reg_t[code.regCount];

    for(int i = 0; i < code.instructions.size(); i++){
        try {
            Process(code.instructions[i]);
        } catch(std::string r){
            Debug::LogError(r);
        }
    }
}

void VM::Process(Instruction ins){

    InstructionID tipus = ins.type;

    switch (tipus)
    {
    case InstructionID::LOADC: //A,B
        registers[ins.GetA()] = currentCode.constants[ins.GetB()].data;
        break;
    // Mas cosas


    case InstructionID::DEF: // De momento es un print
        FunctionMap[Internal::InternalFunctions::PRINT]
            (registers + ins.GetA(), 1);
        break;
    default: // Nose excepcion supongo??? XD
        throw(std::string("Undefined instruction error"));
        break;
    }
}