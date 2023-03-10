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

    stackOffset = 0;

    for(int i = 0; i < code.instructions.size(); i++){
        try {
            Process(code.instructions[i], &i);
        } catch(std::string r){
            Debug::LogError(r);
            break;
        }
    }
}

void VM::Process(Instruction ins, int* current){

    InstructionID tipus = ins.type;
    std::map<InstructionID, std::string> table = {
            {LOAD, "LOAD"},
            {LOADC, "LOADC"},
            {PUSH, "PUSH"},
            {CLOSE, "CLOSE"},
            {CONT, "CONT"},
            {MOVE, "MOVE"},
            {MOVER, "MOVER"},
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
    // Debug::Log(table[tipus]); std::cout << std::endl;
    try {
        switch (tipus)
        {
        case InstructionID::LOAD:
            // std::cout << "S: " << ins.GetB() + stackOffset << " -> R: " << ins.GetA() << std::endl;
            registers[ins.GetA()] = stack[ins.GetB() + stackOffset];
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
        case InstructionID::TEST:
            if(registers[ins.GetA()].num == 0){
                *current += ins.GetB();
            }
            break;
        case InstructionID::JUMP:
            *current += ins.GetA();
            break;
        case InstructionID::MOVE:
            stack[ins.GetB() + stackOffset] = registers[ins.GetA()];
            // std::cout << "S: " << ins.GetB() + stackOffset << " <- R: " << ins.GetA() << std::endl;
            break;
        case InstructionID::CLOSE:
            for(int i = 0; i < ins.GetA(); i++) stack.pop_back();
            // std::cout << "Before: " << stackOffset << std::endl;
            stackOffset = offsets.top();
            offsets.pop();
            // std::cout << "After: " << stackOffset << std::endl;
            break;
        case InstructionID::CONT:
            offsets.push(stackOffset);
            stackOffset = stack.size();

            // std::cout << "OFSET: " << stackOffset << std::endl;
            break;
        // Operacions
        // Arit
        case InstructionID::ADD:
            registers[ins.GetA()] = Operate(Operator::OP_ADD, registers + ins.GetA(), registers + ins.GetB());
            break;
        case InstructionID::MUL:
            registers[ins.GetA()] = Operate(Operator::OP_MUL, registers + ins.GetA(), registers + ins.GetB());
            break;  
        case InstructionID::SUB:
            registers[ins.GetA()] = Operate(Operator::OP_SUB, registers + ins.GetA(), registers + ins.GetB());
            break; 
        case InstructionID::DIV:
            registers[ins.GetA()] = Operate(Operator::OP_DIV, registers + ins.GetA(), registers + ins.GetB());
            break; 
        case InstructionID::MOD:
            registers[ins.GetA()] = Operate(Operator::OP_MOD, registers + ins.GetA(), registers + ins.GetB());
            break;
        // Bool
        case InstructionID::EQ:
            registers[ins.GetA()] = Operate(Operator::OP_EQ, registers + ins.GetA(), registers + ins.GetB());
            break;
        case InstructionID::NEQ:
            registers[ins.GetA()] = Operate(Operator::OP_NEQ, registers + ins.GetA(), registers + ins.GetB());
            break;
        case InstructionID::NOT:
            registers[ins.GetA()] = Operate(Operator::OP_NOT, registers + ins.GetA(), registers + ins.GetB());
            break;
        case InstructionID::LT:
            registers[ins.GetA()] = Operate(Operator::OP_LT, registers + ins.GetA(), registers + ins.GetB());
            break;
        case InstructionID::GT:
            registers[ins.GetA()] = Operate(Operator::OP_GT, registers + ins.GetA(), registers + ins.GetB());
            break;
        case InstructionID::LEQ:
            registers[ins.GetA()] = Operate(Operator::OP_LEQ, registers + ins.GetA(), registers + ins.GetB());
            break;
        case InstructionID::GEQ:
            registers[ins.GetA()] = Operate(Operator::OP_GEQ, registers + ins.GetA(), registers + ins.GetB());
            break;
        case InstructionID::AND:
            registers[ins.GetA()] = Operate(Operator::OP_AND, registers + ins.GetA(), registers + ins.GetB());
            break;
        default: // Nose excepcion supongo??? XD
            throw(std::string("Undefined instruction error " + std::to_string(tipus)));
            break;
        }
    } catch(std::string r){
        throw(r);
    }
    
    // RegDump();
}

reg_t VM::Operate(Operator op, reg_t* a, reg_t* b){
    if(operations.count({op, a->type, b->type})) return operations[{op, a->type, b->type}](a, b);
    throw("No match for operator " + GetOperationName(op) + " and types " 
        + GetTypeName(a->type) + ", " + GetTypeName(b->type));
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