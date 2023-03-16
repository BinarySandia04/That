#include "reinterpreter.h"

#include <string>

#include "../compiler/nodes.h"

using namespace That;

Reinterpreter::Reinterpreter(Nodes::Node *ast){
    this->ast = ast;
}

std::string Reinterpreter::GetCode(){
    return "Hola";
}