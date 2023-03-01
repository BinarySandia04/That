#include "assembler.h"
#include "../flags/flags.h"
#include "../headers/termcolor.hpp"

#include <map>
#include <iostream>
#include <string>
#include <vector>

using namespace That;

void Assembler::Assemble(Nodes::Node* ast, Flag::Flags flags){
    AssembleCode(ast);

    if(CHECK_BIT(flags, 1)){
        std::cout << termcolor::red << termcolor::bold << "ASM:" << termcolor::reset << std::endl;
        // Ara doncs fem debug de les instruccions
        for(int i = 0; i < assembly.size(); i++){
            std::cout << assembly[i].type << " " << assembly[i].a << " " << assembly[i].b << 
            " " << assembly[i].x << std::endl;
        }
        std::cout << std::endl;
    }
    
}

void Assembler::AssembleCode(Nodes::Node* node){
    // Aconseguim el tipus de assembleCode!!!
    
    std::cout << "Miau3 " << node->children.size() << std::endl;
    for(int i = 0; i < node->children.size(); i++){
        Nodes::Node* n = node->children[i];
        Nodes::NodeType t = n->type;
        
        std::cout << t << std::endl;
        if(IsExpression(t)) AssembleExpression(n);
        else if(t == Nodes::DEF_FUNCTION) AssembleDef(n);
        else if(t == Nodes::FUNCTION){
            AssembleFunction(n);
        }
    }
    
}

void Assembler::AssembleFunction(Nodes::Node* func){
            std::cout << "Miau2 " << func->children.size() << std::endl;

    // Tenim aqui que els fills son en ordre:
    // <nom>, <param> x nd x <param>, <codi>
    // Suposant que al stack tenim les anteriors i al registre tenim els paràmetres fem la funció
    // Hem de saber ara doncs quins paràmetres tenim penjats al stack.
    // Com que aixo ho fem al cridar la funció, doncs estaràn enrere suposo. Anem a
    // afegir-los ara virtualment segons els paràmetres que tenim ara a la funció.
    stackPointer = identifierStack.size();
    // ref - name
    // type - return tipus
    // code
    // params, 2 en 2
    std::string funcName = func->children[0]->GetDataString();

    for(int i = 0; i < (func->children.size() - 3) / 2; i++){
        // i + 2 -> tipus, i + 3 -> ref
        AppendReference(func->children[i+3]);
    }

    AssembleCode(func->children[2]);
}

void Assembler::AssembleDeclaration(Nodes::Node *dec){
    // type DEC -> [EXP, TYPE]
    // Primer hauriem de fer un assemble de l'expressió

    // Suposo que hem de fer algo amb el type per optimitzar???
    AssembleExpression(dec->children[0]);
    // Ara l'expression hauria d'estar al top del stack
    Instruction push;
    push.type = VM::PUSH;
    push.a = regPointer;
    push.b = regPointer;

    PushInstruction(push);
}

void Assembler::AssembleExpression(Nodes::Node *exp){
    // Esta molt guai tenim una expression hem de fer coses
    // Podem tenir valors literals, la idea es que el resultat final el tinguem al registre que apunta el nostre punter + 1
    // Hem de tenir en compte que els registres després del punter no tenen efecte

    if(exp->type == Nodes::NodeType::EXP_BINARY){
        // La idea es veure si una de les dues doncs es literal o no
        Nodes::Node* f = exp->children[0], *s = exp->children[1], *t;
        
        // Optimització important
        if(IsValue(f->type)){
            t = s;
            s = f;
            f = t;
        }

        Instruction op;
        op.type = TranslateBinOpId(exp->nd);

        // Val si cap dels dos es valor podem cridar recursivament AssembleExpression amb un dels dos
        AssembleExpression(f);
        // Ara tenim al nostre punter f assembleat. L'augmentem i assemblem t
        op.a = regPointer;
        IncreasePointer();
        AssembleExpression(s);
        op.b = regPointer;
        DecreasePointer();

        PushInstruction(op);
    }
    else if(exp->type == Nodes::NodeType::EXP_UNARY){
        Nodes::Node* f = exp->children[0];
        Instruction op;
        op.type = TranslateUnOpId(exp->nd);
        // Val doncs volem al nostre punter l'expressió
        AssembleExpression(f);
        // I ara li apliquem la operació
        op.a = regPointer;

        PushInstruction(op);
    } else if(IsValue(exp->type)){
        // Bueno carreguem i ja està
        Instruction loadc;
        loadc.type = VM::Instructions::LOADC;
        loadc.a = regPointer;
        loadc.b = GetConstId(exp);

        PushInstruction(loadc);
        return;
    } else if(exp->type == Nodes::NodeType::EXP_CALL){
        AssembleCall(exp);
    }
}

void Assembler::AssembleDef(Nodes::Node* def){
    // El fill és una expression
    Instruction dif;
    dif.type = VM::Instructions::DEF;
    AssembleExpression(def->children[0]);
    dif.a = regPointer;

    PushInstruction(dif);
}

void Assembler::AssembleCall(Nodes::Node *call){
    // Ok suposem que call és una call.
    // Primer carreguem la funció a cridar
    Instruction loadc;
    loadc.type = VM::Instructions::LOADC;
    loadc.a = regPointer;
    loadc.b = GetRefId(call->GetDataString());

    PushInstruction(loadc);
    IncreasePointer();
    // Ara estaria bé carregar tots els paràmetres
    for(int i = 0; i < call->children.size(); i++){
        AssembleExpression(call->children[i]);
        IncreasePointer();
    }
    // Tornem enrere on la funció
    for(int i = 0; i <= call->children.size(); i++) DecreasePointer();

    // Empenyem al stack
    Instruction stackPush;
    stackPush.type = VM::Instructions::PUSH;
    stackPush.a = regPointer + 1;
    stackPush.b = regPointer + call->children.size();

    // Ara cridem a la funció. Hauria de sobreescriure a on es troba en la memoria
    // Com que 
    Instruction cins;
    cins.type = VM::Instructions::CALL;
    cins.a = regPointer;
    cins.b = regPointer + 1;
    cins.x = regPointer + call->children.size();

    PushInstruction(cins);
}

// De moment serà un ADD
VM::Instructions Assembler::TranslateBinOpId(int data){
    return VM::Instructions::ADD;
}

VM::Instructions Assembler::TranslateUnOpId(int data){
    return VM::Instructions::ADD;
}

void Assembler::AppendReference(That::Nodes::Node* ref){
    // Suposem que ref es de tipus referència. Aleshores doncs té un string molt maco!
    std::string id = "";
    for(int i = 0; i < ref->nd; i++){
        id += ref->data.bytes[i];
    }
    
    identifierStack.push_back(id);
}

void Assembler::IncreasePointer(){
    regPointer++;
    if(regPointer >= regCount) regCount = regPointer + 1;
}

void Assembler::DecreasePointer(){
    regPointer--;
}

// TODO: Canviar això per suportar més coses
bool Assembler::IsValue(Nodes::NodeType t){
    return (t == Nodes::NodeType::VAL_BOOLEAN ||
    t == Nodes::NodeType::VAL_INT ||
    t == Nodes::NodeType::VAL_NULL ||
    t == Nodes::NodeType::VAL_REAL ||
    t == Nodes::NodeType::VAL_STRING);
}

bool Assembler::IsExpression(Nodes::NodeType t){
    return (IsValue(t) ||
    t == Nodes::NodeType::EXP_BINARY ||
    t == Nodes::NodeType::EXP_UNARY ||
    t == Nodes::NodeType::EXP_CALL);
}

void Assembler::PushInstruction(Instruction ins){
    assembly.push_back(ins);
}

// TODO: Aixo es de prova
int Assembler::GetConstId(Nodes::Node *val){
    return val->data.integer;
}

int Assembler::GetRefId(std::string ref){
    for(int i = identifierStack.size() - 1; i >= 0; i--){
        if(identifierStack[i] == ref){
            return i - stackPointer;
        }
    }
    // Error
    throw(std::string("Name Error: " + ref + " is not defined."));
}

Instruction::Instruction(){
    this->a = -1;
    this->b = -1;
    this->x = -1;
    this->type = VM::Instructions::HALT;
}