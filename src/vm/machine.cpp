#include <iostream>
#include <climits>

#include "machine.h"
#include "internal.h"
#include "../headers/debug.hpp"
#include "../flags/flags.h"
#include "../headers/termcolor.hpp"

using namespace That;

void VM::MemDump(uint8_t *data, int size){
    std::cout << "[Dump] Size: " << size << std::endl;

    for(int i = 0; i < size; i++){
        std::cout << (int) *(data + i) << " ";
    }
    std::cout << std::endl;
    return;
}

void VM::Run(MachineCode code, Flag::Flags flags){
    // return;
    currentCode = code;

    // Reservar memoria bonita para todo el code de machinecode
    registers = new reg_t[code.regCount];

    debug = CHECK_BIT(flags, 1);

    // Carreguem funcions internes
    Internal::LoadDefaultFunctions(&defaultFunctions);
    Internal::LoadInternalFunctions(&internalFunctions);
    Internal::LoadConversions(&conversions);
    Internal::LoadOperations(&operations);

    for(int i = 0; i < code.instructions.size(); i++){
        try {
            Process(code.instructions[i], &i);
        } catch(std::string r){
            Debug::LogError(r);
            break;
        }
    }

    if(debug) std::cout << "SIZE: " << code.regCount << std::endl;
}

void VM::Process(Instruction ins, int* current){

    InstructionID tipus = ins.type;
    /*
    if(debug){
        std::map<InstructionID, std::string> table = {
            {LOADC, "LOADC"},
            {MOVE, "MOVE"},
            {CALL, "CALL"},
            {DEF, "DEF"},
            {ICL, "ICL"},
            {RET, "RET"},
            {ADD, "ADD"},
            {SUB, "SUB"},
            {MUL, "MUL"},
            {DIV, "DIV"},
            {MOD, "MOD"},
            {AND, "AND"},
            {OR, "OR"},
            {NOT, "NOT"},
            {EQ, "EQ"},
            {NEQ, "NEQ"},
            {GT, "GT"},
            {LT, "LT"},
            {GEQ, "GEQ"},
            {LEQ, "LEQ"},
            {TO, "TO"},
            {END, "END"},
            {JUMP, "JUMP"},
            {TEST, "TEST"},
            {HALT, "HALT"},
        };
    std::cout << termcolor::color<255,125,0> << "(" << *current << ") " << termcolor::reset;
    Debug::Log(table[tipus]); std::cout << std::endl;
    if(ins.GetA() != INT_MIN) Debug::Log(ins.GetA()); std::cout << " ";
    if(ins.GetB() != INT_MIN) Debug::Log(ins.GetB()); std::cout << " ";
    if(ins.GetC() != INT_MIN) Debug::Log(ins.GetC()); std::cout << " " << std::endl;
    }
    */
    
    try {
        switch (tipus)
        {
        case InstructionID::LOADC: //A,B
            // if(debug) std::cout << "C: " << ins.GetB() << " (" << currentCode.constants[ins.GetB()].data.num << ") -> R: " << ins.GetA() << std::endl;
            registers[ins.GetA()] = currentCode.constants[ins.GetB()].data;
            break;
        case InstructionID::DEF: // De momento es un print
            defaultFunctions[0](registers + ins.GetA(), 1);
            break;
        case InstructionID::TEST:
            if(registers[ins.GetA()].num == 0){
                *current += ins.GetB() - 1;
            }
            break;
        case InstructionID::JUMP:
            *current += ins.GetA();
            break;
        case InstructionID::MOVE:
            registers[ins.GetB()] = registers[ins.GetA()];
            break;
        case InstructionID::ADD:
            Operate(Operator::OP_ADD, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::MUL:
            Operate(Operator::OP_MUL, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;  
        case InstructionID::SUB:
            Operate(Operator::OP_SUB, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break; 
        case InstructionID::DIV:
            Operate(Operator::OP_DIV, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break; 
        case InstructionID::MOD:
            Operate(Operator::OP_MOD, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        // Bool
        case InstructionID::EQ:
            Operate(Operator::OP_EQ, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::NEQ:
            Operate(Operator::OP_NEQ, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::NOT:
            Operate(Operator::OP_NOT, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::LT:
            Operate(Operator::OP_LT, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::GT:
            Operate(Operator::OP_GT, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::LEQ:
            Operate(Operator::OP_LEQ, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::GEQ:
            Operate(Operator::OP_GEQ, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::AND:
            Operate(Operator::OP_AND, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        default: // Nose excepcion supongo??? XD
            throw(std::string("Undefined instruction error " + std::to_string(tipus)));
            break;
        }
    } catch(std::string r){
        throw(r);
    }
    
    // if(debug) RegDump();
}

void VM::Operate(Operator op, reg_t* a, reg_t* b, reg_t *c){
    //if(operations.count({op, a->type, b->type})){
    operations[Internal::HashOperation(op, a->type, b->type)](a, b, c);
    return;
    // TODO: Suport per operadors?
}

std::string VM::GetTypeName(Type t){
    std::map<Type, std::string> m = {
        {INT,       "Int"},
        {NUMBER,    "Number"},
        {REAL,      "Real"},
        {STRING,    "String"},
        {BOOL,      "Bool"},
        {_NULL,     "Null"}
    };
    return m[t];
}

std::string VM::GetOperationName(Operator t){
    std::map<Operator, std::string> m = {
        {OP_ADD,       "+"},
        {OP_SUB,    "-"},
        {OP_MUL,      "*"},
        {OP_DIV,    "/"},
        {OP_MOD,      "%"},
        {OP_EQ,      "=="},
        {OP_NEQ,      "!="},
        {OP_NOT,      "!"},
        {OP_GT,      ">"},
        {OP_LT,      "<"},
        {OP_GEQ,      ">="},
        {OP_LEQ,      "<="},
        {OP_AND,      "&&"},
        {OP_OR,      "||"}
    };
    return m[t];
}

void VM::RegDump(){
    for(int i = 0; i < currentCode.regCount; i++){
        std::cout << i << ": [" << GetTypeName(registers[i].type) << ", " << registers[i].num << "]" << std::endl;
    }
}