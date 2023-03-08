#include <iostream>

#include "machine.h"
#include "internal.h"
#include "../headers/debug.hpp"

using namespace That;

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

    // Carreguem funcions internes
    Internal::LoadDefaultFunctions(&defaultFunctions);
    Internal::LoadInternalFunctions(&internalFunctions);
    Internal::LoadConversions(&conversions);
    Internal::LoadOperations(&operations);

    for(int i = 0; i < code.instructions.size(); i++){
        try {
            Process(code.instructions[i]);
        } catch(std::string r){
            Debug::LogError(r);
            break;
        }
    }
}

void VM::Process(Instruction ins){

    InstructionID tipus = ins.type;

    switch (tipus)
    {
    case InstructionID::LOAD:
        registers[ins.GetA()] = stack[ins.GetB()];
        break;
    case InstructionID::LOADC: //A,B
        registers[ins.GetA()] = currentCode.constants[ins.GetB()].data;
        break;
    case InstructionID::PUSH:
        for(int f = ins.GetA(), t = ins.GetB(); f <= t; f++)
        stack.push_back(registers[f]);
        break;
    case InstructionID::DEF: // De momento es un print
        defaultFunctions[0]
            (registers + ins.GetA(), 1);
        break;
    case InstructionID::ADD:
        registers[ins.GetA()] = operations[{Operator::OP_ADD, registers[ins.GetA()].type, registers[ins.GetB()].type}]
            (registers + ins.GetB(), registers + ins.GetA());
        break;
    default: // Nose excepcion supongo??? XD
        throw(std::string("Undefined instruction error"));
        break;
    }
}