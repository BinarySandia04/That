#include "assembler.h"
#include "../flags/flags.h"
#include "../headers/termcolor.hpp"

#include <map>
#include <iostream>
#include <string>
#include <vector>

using namespace That;

void Assembler::Assemble(Nodes::Node* ast, Flag::Flags flags){
    AssembleCode(ast, &assembly);

    if(CHECK_BIT(flags, 1)){
        std::cout << termcolor::red << termcolor::bold << "ASM:" << termcolor::reset << std::endl;
        // Ara doncs fem debug de les instruccions
        for(int i = 0; i < assembly.size(); i++){
            std::cout << assembly[i].type << " ";
            if(assembly[i].a != INT32_MIN) std::cout << assembly[i].a << " ";
            if(assembly[i].b != INT32_MIN) std::cout << assembly[i].b << " ";
            if(assembly[i].x != INT32_MIN) std::cout << assembly[i].x << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    
}

void Assembler::AssembleCode(Nodes::Node* node, std::vector<Instruction> *to){
    
    // Mirem primer quines funcions hi ha al entorn i les identifiquem
    for(int i = 0; i < node->children.size(); i++){
        Nodes::NodeType t = node->children[i]->type;
        if(t == Nodes::FUNCTION){
            AppendReference(node->children[i]->children[0]);
        }
    }

    // Després les definim
    for(int i = 0; i < node->children.size(); i++){
        Nodes::NodeType t = node->children[i]->type;
        if(t == Nodes::FUNCTION){
            AssembleFunction(node->children[i], to);
        }
    }

    // Ara executem    
    for(int i = 0; i < node->children.size(); i++){
        Nodes::Node* n = node->children[i];
        Nodes::NodeType t = n->type;
        
        if(IsExpression(t)) AssembleExpression(n, to);
        else if(t == Nodes::DEF_FUNCTION) AssembleDef(n, to);
        else if(t == Nodes::DECLARATION) AssembleDeclaration(n, to);
        else if(t == Nodes::ASSIGNATION) AssembleAssignation(n, to);
        else if(t == Nodes::RETURN) AssembleReturn(n, to);
        else if(t == Nodes::IF) AssembleConditional(n, to);
        else if(t == Nodes::WHILE) AssembleWhile(n, to);
        else if(t == Nodes::FOR) AssembleFor(n, to);
    }
    
}

void Assembler::AssembleFunction(Nodes::Node* func, std::vector<Instruction> *to){
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
    Instruction top;
    top.type = VM::TO;
    top.a = stackPointer - 1; // Direcció d'aquesta funció
    PushInstruction(top, to);

    for(int i = 0; i <= (func->children.size() - 3) / 2; i++){
        // i + 2 -> tipus, i + 3 -> ref
        // O get reference id
        AppendReference(func->children[i+3]);
        std::cout << "Hola " << identifierStack.size() << std::endl;
    }

    AssembleCode(func->children[2], to);

    // Vale ara que ja tenim la funció anem a treure tot el que haviem suposadament
    // ficat al stack

    while(identifierStack.size() > stackPointer){
        identifierStack.pop_back();
    }

    Instruction end;
    top.type = VM::END;
    PushInstruction(end, to);
}


// TODO: Acabar de fer els ifs
void Assembler::AssembleConditional(Nodes::Node* cond, std::vector<Instruction> *to){
    // If
    // Conditional
    // Code
    // |
    // Conditional
    // Code
    // Code

    int a = 0;
    std::vector<std::vector<Instruction>> conditions, codes;
    for(int i = 0; i < cond->children.size(); i++){
        
        std::cout << i << std::endl;
        cond->children[i]->Debug();
        std::cout << std::endl << std::endl;

        std::vector<Instruction> condit, code;

        if(i % 2 == 0){ // Ultimo codigo o condicion
            if(cond->children.size() % 2 == 1 && i == cond->children.size() - 1){
                // Aislar
                
                stackPointer = identifierStack.size();
                AssembleCode(cond->children[i], &code);
                
                while(identifierStack.size() > stackPointer){
                    identifierStack.pop_back();
                }
                
                codes.push_back(code);
                a += code.size();
            } else {
                AssembleExpression(cond->children[i], &condit);
                conditions.push_back(condit);

                a += condit.size();
            }
            
        } else { // Codigo
            // Aislar
            stackPointer = identifierStack.size();

            AssembleCode(cond->children[i], &code);

            while(identifierStack.size() > stackPointer){
                identifierStack.pop_back();
            }
            
            codes.push_back(code);
            a += code.size() + 1;
            // if(cond->children.size() % 2 == 1) a++;
            a++;
        }
    }

    std::cout << "A: " << a << std::endl;
    

    // Val si es parell la idea es saltar fins al final
    for(int i = 0; i < cond->children.size(); i++){
        
        std::cout << i << std::endl;
        if(cond->children.size() % 2 == 1 && i == cond->children.size() - 1){
            PushInstructions(&codes[(i) / 2], to);
            std::cout << "Else??? " << i << " " << codes[(i) / 2].size() << std::endl;
            break;
        }

        if(i % 2 == 0){
            a -= conditions[i / 2].size();
            PushInstructions(&conditions[i / 2], to);

            a--;
            Instruction test;
            test.type = VM::TEST;
            test.a = regPointer;
            
            test.b = codes[(i / 2)].size() + 1;
            /*
            if(cond->children.size() % 2 == 1) test.b = a - codes[codes.size() - 1].size(); // Saltem al else
            else{
                test.b = a; // Saltem al final
            }
            */
            
            PushInstruction(test, to);

            
        } else {
            PushInstructions(&codes[(i - 1) / 2], to);
            a -= codes[(i - 1) / 2].size();

            //if(cond->children.size() % 2 == 1){
                a--;
                Instruction jmp;
                jmp.type = VM::JUMP;
                jmp.a = a;
                PushInstruction(jmp, to);
            //}
        }
    }
    
}

void Assembler::AssembleFor(Nodes::Node* para, std::vector<Instruction> *to){
    
}

// TODO: Falta aillar contexto
void Assembler::AssembleWhile(Nodes::Node* whil, std::vector<Instruction> *to){
    std::vector<Instruction> exp, code;
    AssembleExpression(whil->children[0], &exp);
    PushInstructions(&exp, to);
    
    // Aislar
    stackPointer = identifierStack.size();
    AssembleCode(whil->children[1], &code);
    while(identifierStack.size() > stackPointer){
        identifierStack.pop_back();
    }

    int n = exp.size() + code.size() + 1;

    Instruction test;
    test.type = VM::TEST;
    test.a = regPointer;
    test.b = code.size() + 1;

    PushInstruction(test, to);
    PushInstructions(&code, to);

    Instruction jump;
    jump.type = VM::JUMP;
    jump.a = -n - 1;

    PushInstruction(jump, to);
}

void Assembler::AssembleReturn(Nodes::Node* ret, std::vector<Instruction> *to){
    AssembleExpression(ret->children[0], to);

    Instruction retu;
    retu.type = VM::RET;
    retu.a = regPointer;

    PushInstruction(retu, to);
}

void Assembler::AssembleDeclaration(Nodes::Node *dec, std::vector<Instruction> *to){
    // type DEC -> [EXP, TYPE]
    // Primer hauriem de fer un assemble de l'expressió

    // Suposo que hem de fer algo amb el type per optimitzar???
    AssembleExpression(dec->children[0], to);
    // Ara l'expression hauria d'estar al top del stack
    Instruction push;
    push.type = VM::PUSH;
    push.a = regPointer;
    push.b = regPointer;

    AppendReference(dec);

    PushInstruction(push, to);
}

void Assembler::AssembleAssignation(Nodes::Node* assign, std::vector<Instruction> *to){
    // Ok primer l'expressió
    int where = GetRefId(assign->GetDataString());
    
    AssembleExpression(assign->children[0], to);

    Instruction move;
    move.type = VM::MOVE;

    move.a = regPointer;
    move.b = where;

    PushInstruction(move, to);

}

void Assembler::AssembleExpression(Nodes::Node *exp, std::vector<Instruction> *to){
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
        AssembleExpression(f, to);
        // Ara tenim al nostre punter f assembleat. L'augmentem i assemblem t
        op.a = regPointer;
        IncreasePointer();
        AssembleExpression(s, to);
        op.b = regPointer;
        DecreasePointer();

        PushInstruction(op, to);
    }
    else if(exp->type == Nodes::NodeType::EXP_UNARY){
        Nodes::Node* f = exp->children[0];
        Instruction op;
        op.type = TranslateUnOpId(exp->nd);
        // Val doncs volem al nostre punter l'expressió
        AssembleExpression(f, to);
        // I ara li apliquem la operació
        op.a = regPointer;

        PushInstruction(op, to);
    } else if(IsValue(exp->type)){
        // Bueno carreguem i ja està
        Instruction loadc;
        loadc.type = VM::Instructions::LOADC;
        loadc.a = regPointer;
        loadc.b = GetConstId(exp);

        PushInstruction(loadc, to);
        return;
    } else if(exp->type == Nodes::NodeType::EXP_CALL){
        AssembleCall(exp, to);
    } else if(exp->type == Nodes::NodeType::REFERENCE){
        int ref = GetRefId(exp->GetDataString());
        Instruction load;
        load.type = VM::Instructions::LOAD;
        load.a = regPointer;
        load.b = ref;

        PushInstruction(load, to);
    }
}

void Assembler::AssembleDef(Nodes::Node* def, std::vector<Instruction> *to){
    // El fill és una expression
    Instruction dif;
    dif.type = VM::Instructions::DEF;
    AssembleExpression(def->children[0], to);
    dif.a = regPointer;

    PushInstruction(dif, to);
}

void Assembler::AssembleCall(Nodes::Node *call, std::vector<Instruction> *to){
    // Ok suposem que call és una call.
    // Primer carreguem la funció a cridar
    Instruction loadc;
    loadc.type = VM::Instructions::LOADC;
    loadc.a = regPointer;
    loadc.b = GetRefId(call->GetDataString());

    PushInstruction(loadc, to);
    IncreasePointer();
    // Ara estaria bé carregar tots els paràmetres
    for(int i = 0; i < call->children.size(); i++){
        AssembleExpression(call->children[i], to);
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

    PushInstruction(cins, to);
}

// De moment serà una resta chunga
VM::Instructions Assembler::TranslateBinOpId(int data){
    return (VM::Instructions) ((int) VM::Instructions::ADD + data - 5);
}

VM::Instructions Assembler::TranslateUnOpId(int data){
    return VM::Instructions::ADD;
}

void Assembler::AppendReference(That::Nodes::Node* ref){
    // Suposem que ref es de tipus referència. Aleshores doncs té un string molt maco!
    std::string id = ref->GetDataString();
    
    identifierStack.push_back(id);
    
    std::cout << "Appended: " << id << std::endl;
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

void Assembler::PushInstruction(Instruction ins, std::vector<Instruction> *where){
    where->push_back(ins);
}

void Assembler::PushInstructions(std::vector<Instruction> *from, std::vector<Instruction> *to){
    for(int i = 0; i < from->size(); i++){
        PushInstruction((*from)[i], to);
    }
}

// TODO: Aixo es de prova
int Assembler::GetConstId(Nodes::Node *val){
    return val->data.integer;
}

int Assembler::GetRefId(std::string ref){
    for(int i = identifierStack.size() - 1; i >= 0; i--){
        if(identifierStack[i] == ref){
            std::cout << ref << std::endl;
            return i - stackPointer;
        }
    }
            std::cout << ref << std::endl;
    // Error
    throw(std::string("Name Error: " + ref + " is not defined."));
}

Instruction::Instruction(){
    this->a = INT32_MIN;
    this->b = INT32_MIN;
    this->x = INT32_MIN;
    this->type = VM::Instructions::HALT;
}