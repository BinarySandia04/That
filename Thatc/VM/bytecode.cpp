#include "bytecode.h"

#include <iostream>
#include <That/logs.h>

using namespace That;
using namespace Thatc;

Bytecode::Bytecode() {

}

Bytecode::~Bytecode() {
  /*
  for(int i = 0; i < instructions.size(); i++){
    delete instructions[i];
  }
  */
  instructions.clear();
}

void Bytecode::print() {
  Logs::Print("CONSTANTS:");
  for(int i = 0; i < constants.size(); i++){
    Logs::Printf("%4d -> ", i);
    constants[i]->print();
    Logs::Printf("\n");
  }
  Logs::Print("INSTRUCTIONS:");
  for(int i = 0; i < instructions.size(); i++){
    Logs::Printf("%4d -> ", i);
    instructions[i]->print();
    Logs::Printf("\n");
  }
}

int Bytecode::pushInstruction(Instruction* instruction){
  instructions.push_back(instruction);
  return 0;
}

int Bytecode::pushConstant(Constant* constant){
  constants.push_back(constant);
  return 0;
}
