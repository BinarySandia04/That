#include "assembler.h"
#include "../flags/flags.h"

#include <map>
#include <iostream>
#include <string>
#include <vector>

using namespace That;

void Assembler::Assemble(Nodes::Node* ast, Flag::Flags flags){
    AssembleFunction(ast->children[0]);
}

void Assembler::AssembleFunction(Nodes::Node* func){
    // Tenim aqui que els fills son en ordre:
    // <nom>, <param> x nd x <param>, <codi>
}

void Assembler::AssembleDeclaration(Nodes::Node *dec){
    // type DEC -> [EXP, TYPE]
    // Primer hauriem de fer un assemble de l'expressió
    AssembleExpression(dec->children[0]);
    // Ara l'expression hauria d'estar al top del stack
}

void Assembler::AssembleExpression(Nodes::Node *exp){

}

void Assembler::AppendReference(That::Nodes::Node* ref){
    // Suposem que ref es de tipus referència. Aleshores doncs té un string molt maco!
    std::string id = "";
    for(int i = 0; i < ref->nd; i++){
        id += ref->data.bytes[i];
    }

    int d = identifiers.size();

    Identifier nId;
    nId.name = id;

    identifiers.push_back(nId);
}

void Assembler::PushExpression(Nodes::Node* exp, std::vector<Instruction> set){
    
}