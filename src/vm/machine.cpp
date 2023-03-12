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
    currentCode = code;

    // Reservar memoria bonita para todo el code de machinecode
    registers = new reg_t[code.regCount];

    debug = CHECK_BIT(flags, 1);

    // Carreguem funcions internes
    Internal::LoadDefaultFunctions(&defaultFunctions);
    Internal::LoadInternalFunctions(&internalFunctions);
    Internal::LoadConversions(&conversions);
    Internal::LoadOperations(&operations);

    offsets.push(0);
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
    if(debug){
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
    std::cout << termcolor::color<255,125,0> << "(" << *current << ") " << termcolor::reset;
    Debug::Log(table[tipus]); std::cout << std::endl;
    if(ins.GetA() != INT_MIN) Debug::Log(ins.GetA()); std::cout << " ";
    if(ins.GetB() != INT_MIN) Debug::Log(ins.GetB()); std::cout << " ";
    if(ins.GetC() != INT_MIN) Debug::Log(ins.GetC()); std::cout << " " << std::endl;
    }
    
    if(debug) StackDump();
    try {
        switch (tipus)
        {
        case InstructionID::LOAD:
            if(debug) std::cout << "S: " << ins.GetB() << " (+" << offsets.top() << ") -> R: " << ins.GetA() << std::endl;
            registers[ins.GetA()] = stack[ins.GetB() + offsets.top()];
            break;
        case InstructionID::LOADC: //A,B
            if(debug) std::cout << "C: " << ins.GetB() << " (" << currentCode.constants[ins.GetB()].data.num << ") -> R: " << ins.GetA() << std::endl;
            registers[ins.GetA()] = currentCode.constants[ins.GetB()].data;
            break;
        case InstructionID::PUSH:
            for(int f = ins.GetA(), t = ins.GetB(); f <= t; f++)
            stack.push_back(registers[f]);

            // if(debug) StackDump();
            break;
        case InstructionID::DEF: // De momento es un print
            defaultFunctions[0]
                (registers + ins.GetA(), 1);
            break;
        case InstructionID::TEST:
            if(registers[ins.GetA()].num == 0){
                *current += ins.GetB() - 1;
            }
            break;
        case InstructionID::JUMP:
            *current += ins.GetA();
            for(int j = 0; j < ins.GetB(); j++){
                for(int i = 0; i < stack.size() - offsets.top(); i++) stack.pop_back();
                offsets.pop();
            }
            break;
        case InstructionID::MOVE:
            stack[ins.GetB() + offsets.top()] = registers[ins.GetA()];
            if(debug) std::cout << "S: " << ins.GetB() + offsets.top() << " <- R: " << ins.GetA() << std::endl;
            break;
        case InstructionID::CLOSE:
            // for(int i = 0; i < ins.GetA(); i++) stack.pop_back();
            //std::cout << offsets.size() << std::endl;
            //std::cout << "Miau: " << stack.size() - offsets.top() << std::endl;
            for(int j = 0; j < ins.GetA(); j++){
                for(int i = 0; i < stack.size() - offsets.top(); i++) stack.pop_back();
                offsets.pop();
            }
            
            break;
        case InstructionID::CONT:
            offsets.push(stack.size());
            // stackOffset = offsets.size();
            
            // std::cout << "Stack size: " << offsets.size() << " | Current: " << offsets.top() << std::endl;
            // if(debug) std::cout << "OFSET: " << offsets.top() << std::endl;
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
    
    if(debug) RegDump();
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

void VM::StackDump(){
    for(int i = 0; i < stack.size(); i++){
        std::cout << termcolor::blue << i << ": [" << GetTypeName(stack[i].type) << ", " << stack[i].num << "]" << termcolor::reset << std::endl;
    }
}