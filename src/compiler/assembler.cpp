#include "assembler.h"
#include "../flags/flags.h"
#include "../headers/termcolor.hpp"
#include "../headers/debug.hpp"
#include "../vm/data.h"

#include <map>
#include <iostream>
#include <string>
#include <vector>

using namespace That;

Assembler::Assembler(Nodes::Node* ast){
    try {
        AssembleCode(ast, &instructions);
    } catch(std::string r){
        Debug::LogError(r);
    }
}

MachineCode Assembler::GetAssembly(){
    MachineCode machine;

    machine.instructions = instructions;
    machine.constants = constants;
    machine.regCount = regCount;

    return machine;
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
        
        try {
            if(IsExpression(t)) AssembleExpression(n, to);
            else if(t == Nodes::DEF_FUNCTION) AssembleDef(n, to);
            else if(t == Nodes::DECLARATION) AssembleDeclaration(n, to);
            else if(t == Nodes::ASSIGNATION) AssembleAssignation(n, to);
            else if(t == Nodes::RETURN) AssembleReturn(n, to);
            else if(t == Nodes::IF) AssembleConditional(n, to);
            else if(t == Nodes::WHILE) AssembleWhile(n, to);
            else if(t == Nodes::FOR) AssembleFor(n, to);
            else if(t == Nodes::BREAK) AssembleTempBreak(n, to);
            else if(t == Nodes::SKIP) AssembleTempSkip(n, to);
        } catch(std::string r){
            throw(r);
        }
        
    }
    
}

void Assembler::AssembleFunction(Nodes::Node* func, std::vector<Instruction> *to){
    // Tenim aqui que els fills son en ordre:
    // <nom>, <param> x nd x <param>, <codi>
    // Suposant que al stack tenim les anteriors i al registre tenim els paràmetres fem la funció
    // Hem de saber ara doncs quins paràmetres tenim penjats al stack.
    // Com que aixo ho fem al cridar la funció, doncs estaràn enrere suposo. Anem a
    // afegir-los ara virtualment segons els paràmetres que tenim ara a la funció.
    
    int stack = StartContext(to);
    // ref - name
    // type - return tipus
    // code
    // params, 2 en 2
    Instruction top(InstructionID::TO, ParamType::E);
    PushInstruction(top, to);
    

    for(int i = 3; i < func->children.size(); i++){
        // i + 2 -> tipus, i + 3 -> ref
        // O get reference id
        AppendReference(func->children[i]);
    }

    AssembleCode(func->children[2], to);

    // Vale ara que ja tenim la funció anem a treure tot el que haviem suposadament
    // ficat al stack
    
    EndContext(stack, to);

    Instruction end(InstructionID::END, ParamType::E);
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
                int stack = StartContext(&code);

                AssembleCode(cond->children[i], &code);
                
                EndContext(stack, &code);
                
                codes.push_back(code);
                a += code.size();
            } else {
                AssembleExpression(cond->children[i], &condit);
                conditions.push_back(condit);

                a += condit.size();
            }
            
        } else { // Codigo
            // Aislar
            int stack = StartContext(&code);

            AssembleCode(cond->children[i], &code);

            EndContext(stack, &code);

            codes.push_back(code);
            a += code.size() + 1;
            // if(cond->children.size() % 2 == 1) a++;
            a++;
        }
    }

    // Val si es parell la idea es saltar fins al final
    for(int i = 0; i < cond->children.size(); i++){
        
        std::cout << i << std::endl;
        if(cond->children.size() % 2 == 1 && i == cond->children.size() - 1){
            PushInstructions(&codes[(i) / 2], to);
            break;
        }

        if(i % 2 == 0){
            a -= conditions[i / 2].size();
            PushInstructions(&conditions[i / 2], to);

            a--;
            Instruction test(InstructionID::TEST, ParamType::AB);

            test.SetA(regPointer);
            
            test.SetB(codes[(i / 2)].size() + 1);
            
            PushInstruction(test, to);

            
        } else {
            PushInstructions(&codes[(i - 1) / 2], to);
            a -= codes[(i - 1) / 2].size();

            a--;
            Instruction jmp(InstructionID::JUMP, ParamType::AB);
            jmp.SetA(a);
            jmp.SetB(0);
            PushInstruction(jmp, to);
        }
    }
    
}

void Assembler::AssembleFor(Nodes::Node* para, std::vector<Instruction> *to){
    // Assemblem primer doncs una declaració, per això, aillem
    int stack = StartContext(to);

    std::vector<Instruction> exp, inc, code, total;
    // Ponemos inicializacion y tal
    AssembleCode(para->children[0], &total);
    int decSize = total.size();

    int a = 0;
    AssembleExpression(para->children[1], &exp);
    a += exp.size() + 1;

    AssembleCode(para->children[2], &inc);
    a += inc.size() + 1;

    AssembleCode(para->children[3], &code);
    a += code.size();

    Instruction jump(InstructionID::JUMP, ParamType::AB), 
        test(InstructionID::TEST, ParamType::AB);
    test.SetA(regPointer);
    test.SetB(a - exp.size());

    PushInstructions(&exp, &total);
    PushInstruction(test, &total);
    PushInstructions(&code, &total);
    PushInstructions(&inc, &total);

    jump.SetA(-a + 1);
    jump.SetB(0);

    PushInstruction(jump, &total);
    std::cout << termcolor::green << "STACK: " << stack << std::endl;
    int ended = EndContext(stack, &total);

    // Val ok ara fem
    int p = total.size();
    for(int i = 0; i < total.size(); i++){
        if(total[i].type == InstructionID::JUMP && total[i].temp == 1){
            // Val eh hem de posar el nombre de salts fin al final ja que això és un break
            total[i].temp = 0;
            total[i].SetA(total.size() - i);
            total[i].SetB(ended);
        }

        if(total[i].type == InstructionID::JUMP && total[i].temp == 2){
            // Val eh ara aixo es un continue
            total[i].temp = 0;
            total[i].SetA(-i + decSize);
            total[i].SetB(0);
        }
    }

    PushInstructions(&total, to);
}

// TODO: Falta aillar contexto
void Assembler::AssembleWhile(Nodes::Node* whil, std::vector<Instruction> *to){
    std::vector<Instruction> exp, code, total;

    int stack = StartContext(&exp);
    
    AssembleExpression(whil->children[0], &exp);
    
    // Aislar
    PushInstructions(&exp, &total);

    AssembleCode(whil->children[1], &code);

    int end = EndContext(stack, &code);

    int n = exp.size() + code.size() + 1;

    Instruction test(InstructionID::TEST, ParamType::AB);
    test.SetA(regPointer);
    test.SetB(code.size() + 2);

    PushInstruction(test, &total);
    PushInstructions(&code, &total);

    Instruction jump(InstructionID::JUMP, ParamType::AB);
    jump.SetA(-n - 1);
    jump.SetB(0);

    PushInstruction(jump, &total);

    int p = total.size();
    for(int i = 0; i < total.size(); i++){
        if(total[i].type == InstructionID::JUMP && total[i].temp == 1){
            // Val eh hem de posar el nombre de salts fin al final ja que això és un break
            total[i].temp = 0;
            total[i].SetA(total.size() - i);
            total[i].SetB(end);
        }

        if(total[i].type == InstructionID::JUMP && total[i].temp == 2){
            // Val eh ara aixo es un continue
            total[i].temp = 0;
            total[i].SetA(-n - 1 + i);
            total[i].SetB(0);
        }
    }

    PushInstructions(&total, to);
}

void Assembler::AssembleReturn(Nodes::Node* ret, std::vector<Instruction> *to){
    AssembleExpression(ret->children[0], to);

    Instruction retu(InstructionID::RET, ParamType::A);
    retu.SetA(regPointer);

    PushInstruction(retu, to);
}

void Assembler::AssembleTempBreak(Nodes::Node *stop, std::vector<Instruction> *to){
    Instruction tmpStop(InstructionID::JUMP, ParamType::AB);
    tmpStop.temp = 1; // Identifier del break

    PushInstruction(tmpStop, to);
}

void Assembler::AssembleTempSkip(Nodes::Node *skip, std::vector<Instruction> *to){
    Instruction tmpSkip(InstructionID::JUMP, ParamType::AB);
    tmpSkip.temp = 2; // Identifier del skip

    PushInstruction(tmpSkip, to);
}

void Assembler::AssembleDeclaration(Nodes::Node *dec, std::vector<Instruction> *to){
    // type DEC -> [EXP, TYPE]
    // Primer hauriem de fer un assemble de l'expressió

    // Suposo que hem de fer algo amb el type per optimitzar???
    try {
        AssembleExpression(dec->children[0], to);
    } catch(std::string r){
        throw(r);
    }
    // Ara l'expression hauria d'estar al top del stack
    Instruction push(InstructionID::PUSH, ParamType::AB);
    push.SetA(regPointer);
    push.SetB(regPointer);

    AppendReference(dec);

    PushInstruction(push, to);
}

void Assembler::AssembleAssignation(Nodes::Node* assign, std::vector<Instruction> *to){
    // Ok primer l'expressió
    int where;
    try {
        where = GetRefId(assign->GetDataString());
    } catch(std::string ex){
        throw(ex);
    }
    
    AssembleExpression(assign->children[0], to);

    Instruction move(InstructionID::MOVE, ParamType::AB);

    move.SetA(regPointer);
    move.SetB(where);

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
        /*if(IsValue(f->type)){
            t = s;
            s = f;
            f = t;
        }*/

        Instruction op(TranslateBinOpId(exp->nd), ParamType::AB);
        // Val si cap dels dos es valor podem cridar recursivament AssembleExpression amb un dels dos
        AssembleExpression(f, to);
        // Ara tenim al nostre punter f assembleat. L'augmentem i assemblem t
        op.SetA(regPointer);
        IncreasePointer();
        AssembleExpression(s, to);
        op.SetB(regPointer);
        DecreasePointer();

        PushInstruction(op, to);
    }
    else if(exp->type == Nodes::NodeType::EXP_UNARY){
        Nodes::Node* f = exp->children[0];
        Instruction op(TranslateUnOpId(exp->nd), ParamType::A);
        // Val doncs volem al nostre punter l'expressió
        AssembleExpression(f, to);
        // I ara li apliquem la operació
        op.SetA(regPointer);

        PushInstruction(op, to);
    } else if(IsValue(exp->type)){
        // Bueno carreguem i ja està
        Instruction loadc(InstructionID::LOADC, ParamType::AB);
        loadc.SetA(regPointer);
        loadc.SetB(GetConstId(exp));

        PushInstruction(loadc, to);
        return;
    } else if(exp->type == Nodes::NodeType::EXP_CALL){
        AssembleCall(exp, to);
    } else if(exp->type == Nodes::NodeType::REFERENCE){
        int ref;
        try {
            ref = GetRefId(exp->GetDataString());
        } catch(std::string ex){
            throw(ex);
        }
        Instruction load(InstructionID::LOAD, ParamType::AB);
        load.SetA(regPointer);
        load.SetB(ref);

        PushInstruction(load, to);
    }
}

void Assembler::AssembleDef(Nodes::Node* def, std::vector<Instruction> *to){
    // El fill és una expression
    Instruction dif(InstructionID::DEF, ParamType::A);
    AssembleExpression(def->children[0], to);
    dif.SetA(regPointer);

    PushInstruction(dif, to);
}

void Assembler::AssembleCall(Nodes::Node *call, std::vector<Instruction> *to){
    // Ok suposem que call és una call.
    // Primer carreguem la funció a cridar
    Instruction loadc(InstructionID::LOADC, ParamType::AB);
    loadc.SetA(regPointer);
    
    try {
        loadc.SetB(GetRefId(call->GetDataString()));
    } catch(std::string s){
        throw(s);
    }

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
    Instruction stackPush(InstructionID::PUSH, ParamType::AB);
    stackPush.SetA(regPointer + 1);
    stackPush.SetB(regPointer + call->children.size());

    // Ara cridem a la funció. Hauria de sobreescriure a on es troba en la memoria
    // Com que 
    Instruction cins(InstructionID::CALL, ParamType::ABC);
    cins.SetA(regPointer);
    cins.SetB(regPointer + 1);
    cins.SetC(regPointer + call->children.size());

    PushInstruction(cins, to);
}

// De moment serà una resta chunga
InstructionID Assembler::TranslateBinOpId(int data){
    return (InstructionID) ((int) InstructionID::ADD + data - 5);
}

InstructionID Assembler::TranslateUnOpId(int data){
    return InstructionID::ADD;
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
    t == Nodes::NodeType::VAL_BIGINT ||
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
    // Vale aqui tenim tot el tema de constants i tal.
    // Hem de fer un reg_t per dir doncs quines constants són i tal
    Constant c;
    reg_t data;
    // Ara fem switch segons el que sigui val. Es un VAL_ALGO
    switch(val->type){
        case Nodes::VAL_INT:
            // Aqui data és int
            data.num = val->data.integer;
            data.type = Type::INT;
            break;
        case Nodes::VAL_BIGINT:
            data.num = val->nd;
            data.data = (uint8_t *) val->data.bytes;

            data.type = Type::NUMBER;
            break;
        case Nodes::VAL_BOOLEAN:
            data.num = val->nd,
            data.type = Type::BOOL;
            break;
        case Nodes::VAL_STRING:
            data.num = val->nd;
            data.data = (uint8_t *) val->data.bytes;

            data.type = Type::STRING;
            break;
        case Nodes::VAL_REAL:
            data.num = val->nd;
            data.data = (uint8_t *) val->data.bytes;

            data.type = Type::REAL;
            break;
        case Nodes::VAL_NULL:
            data.num = val->nd;
            data.type = Type::_NULL;
            break;
        default:
            break;
    }
    c.data = data;

    // Val ja tenim la constant ara la busquem!!!
    int i;
    for(i = 0; i < constants.size(); i++){
        // Comparem constants[i].data amb val->data
        bool eq = true;
        if(constants[i].data.type == data.type){
            if(constants[i].data.num == data.num){
                // Val mirem si el tipus té mes data
                if(data.type == Type::NUMBER || data.type == Type::STRING || data.type == Type::REAL){
                    for(int j = 0; j < data.num; j++){
                        if(constants[i].data.data[j] != data.data[j]){
                            std::cout << "Si, son 2" << std::endl;
                            eq = false;
                            break;
                        }
                    }
                }
                

                if(eq){
                    // Son iguals
                    // std::cout << "I (iguals): " << i << std::endl;
                    return i;
                }
            }
        }
        // No són iguals
    }
    // std::cout << "Push back!" << std::endl;
    constants.push_back(c);
    // std::cout << "I: " << i << std::endl;
    return i;
}


int Assembler::StartContext(std::vector<Instruction> *to){
    Instruction cont(InstructionID::CONT, That::ParamType::A);
    to->push_back(cont);
    stackPointer = identifierStack.size();
    return stackPointer;
}

// TODO: Falta alguna manera para decir a la maquina virtual de hacer close
int Assembler::EndContext(int from, std::vector<Instruction> *to){
    int n = 0;
    while(identifierStack.size() > from){
        identifierStack.pop_back();
        n++;
    }
    Debug::LogImportant(n);
    std::cout << "\n";
    stackPointer = from;

    Instruction close(InstructionID::CLOSE, ParamType::A);
    close.SetA(n);
    to->push_back(close);

    return n;
}

int Assembler::GetRefId(std::string ref){
    for(int i = identifierStack.size() - 1; i >= 0; i--){
        if(identifierStack[i] == ref){
            std::cout << "REF: " << ref << " " << i - stackPointer + 1 << " S: " << stackPointer <<  std::endl;
            return i - stackPointer;
        }
    }
    //std::cout << ref << std::endl;
    // Error
    throw(std::string("Name Error: " + ref + " is not defined."));
}