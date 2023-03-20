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

VM::VM(Flag::Flags flags){
    this->flags = flags;
    debug = CHECK_BIT(flags, 1);

    // Carreguem funcions internes
    Internal::LoadDefaultFunctions(&defaultFunctions);
    Internal::LoadInternalFunctions(&internalFunctions);
    Internal::LoadConversions(&conversions);
    Internal::LoadOperations(&operations);
}

void VM::Run(MachineCode code){
    // return;
    reg_t* registers;
    registers = new reg_t[code.regCount];

    for(int i = 0; i < code.instructions.size(); i++){
        try {
            Process(code.instructions[i], &i, &code.constants, registers);
        } catch(std::string r){
            Debug::LogError(r);
            break;
        }
    }

    if(debug) std::cout << "SIZE: " << code.regCount << std::endl;
    delete[] registers;
}

void VM::Process(Instruction ins, int* current, std::vector<Constant> *constants, reg_t* registers){

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
            registers[ins.GetA()] = (*constants)[ins.GetB()].data;
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
        case InstructionID::CALL: // A: func, B --- C: params
            // A veure aqui caldria eh aconseguir la funció i constants de A i doncs eecutar recursivament i tal
            break;

        case InstructionID::ADD:
            Operate(OpType::OP_ADD, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::MUL:
            Operate(OpType::OP_MUL, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;  
        case InstructionID::SUB:
            Operate(OpType::OP_SUBTRACT, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break; 
        case InstructionID::DIV:
            Operate(OpType::OP_DIV, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break; 
        case InstructionID::MOD:
            Operate(OpType::OP_MOD, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        // Bool
        case InstructionID::EQ:
            Operate(OpType::OP_EQ, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::NEQ:
            Operate(OpType::OP_NEQ, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::NOT:
            Operate(OpType::OP_NOT, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::LT:
            Operate(OpType::OP_LT, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::GT:
            Operate(OpType::OP_GT, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::LEQ:
            Operate(OpType::OP_LEQ, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::GEQ:
            Operate(OpType::OP_GEQ, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
            break;
        case InstructionID::AND:
            Operate(OpType::OP_AND, registers + ins.GetA(), registers + ins.GetB(), registers + ins.GetC());
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

void VM::Operate(OpType op, reg_t* a, reg_t* b, reg_t *c){
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

std::string VM::GetOperationName(OpType t){
    std::map<OpType, std::string> m = {
        {OP_ADD,       "+"},
        {OP_SUBTRACT,    "-"},
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

void VM::RegDump(reg_t* regs, int s){
    for(int i = 0; i < s; i++){
        std::cout << i << ": [" << GetTypeName(regs[i].type) << ", " << regs[i].num << "]" << std::endl;
    }
}