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
        std::vector<int> refs;
        AssembleCode(ast, &instructions, &refs);
    } catch(std::string r){
        Debug::LogError(r);
    }
}

MachineCode Assembler::GetAssembly(){
    MachineCode machine;

    machine.instructions = instructions;
    machine.constants = constants;
    machine.regCount = reserves.size();

    return machine;
}

void Assembler::AssembleCode(Nodes::Node* node, std::vector<Instruction> *to, std::vector<int> *declared){

    // Mirem primer quines funcions hi ha al entorn i les identifiquem
    /*
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
    */

    // Ara executem    
    for(int i = 0; i < node->children.size(); i++){
        Nodes::Node* n = node->children[i];
        Nodes::NodeType t = n->type;
        
        try {
            if(n->IsExpression()) AssembleExpression(n, to);
            else if(t == Nodes::DEF_FUNCTION) AssembleDef(n, to);
            else if(t == Nodes::DECLARATION) declared->push_back(AssembleDeclaration(n, to));
            else if(t == Nodes::ASSIGNATION) AssembleAssignation(n, to);
            // else if(t == Nodes::RETURN) AssembleReturn(n, to);
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

/*
void Assembler::AssembleFunction(Nodes::Node* func, std::vector<Instruction> *to){
    // Tenim aqui que els fills son en ordre:
    // <nom>, <param> x nd x <param>, <codi>
    // Suposant que al stack tenim les anteriors i al registre tenim els paràmetres fem la funció
    // Hem de saber ara doncs quins paràmetres tenim penjats al stack.
    // Com que aixo ho fem al cridar la funció, doncs estaràn enrere suposo. Anem a
    // afegir-los ara virtualment segons els paràmetres que tenim ara a la funció.
    
    StartContext(to);
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
    
    EndContext(to);

    Instruction end(InstructionID::END, ParamType::E);
    PushInstruction(end, to);
}
*/


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
    std::vector<int> conditionLoc;
    for(int i = 0; i < cond->children.size(); i++){
        
        // std::cout << i << std::endl;
        // cond->children[i]->Debug();
        // std::cout << std::endl << std::endl;

        std::vector<Instruction> condit, code;

        if(i % 2 == 0){ // Ultimo codigo o condicion
            if(cond->children.size() % 2 == 1 && i == cond->children.size() - 1){
                // Aislar

                std::vector<int> cont;
                AssembleCode(cond->children[i], &code, &cont);
                FreeContext(&cont);
                
                codes.push_back(code);
                a += code.size();
            } else {
                int r = AssembleExpression(cond->children[i], &condit);
                conditionLoc.push_back(r);
                conditions.push_back(condit);

                a += condit.size();
            }
            
        } else { // Codigo
            // Aislar
            
            std::vector<int> cont;
            AssembleCode(cond->children[i], &code, &cont);
            FreeContext(&cont);

            codes.push_back(code);
            a += code.size() + 1;
            // if(cond->children.size() % 2 == 1) a++;
            a++;
        }
    }

    // Val si es parell la idea es saltar fins al final
    for(int i = 0; i < cond->children.size(); i++){
        
        // std::cout << i << std::endl;
        if(cond->children.size() % 2 == 1 && i == cond->children.size() - 1){
            PushInstructions(&codes[(i) / 2], to);
            break;
        }

        if(i % 2 == 0){
            a -= conditions[i / 2].size();
            PushInstructions(&conditions[i / 2], to);

            a--;
            Instruction test(InstructionID::TEST, ParamType::AB);

            test.SetA(conditionLoc[i/2]);
            
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
    std::vector<int> cont;

    std::vector<Instruction> exp, inc, code, total;
    // Ponemos inicializacion y tal
    AssembleCode(para->children[0], &total, &cont);
    int decSize = total.size();

    int a = 0;
    int expLoc = AssembleExpression(para->children[1], &exp);
    a += exp.size() + 1;

    AssembleCode(para->children[2], &inc, &cont);
    a += inc.size() + 1;

    AssembleCode(para->children[3], &code, &cont);
    a += code.size();

    Instruction jump(InstructionID::JUMP, ParamType::AB), 
        test(InstructionID::TEST, ParamType::AB);
    test.SetA(expLoc);
    test.SetB(a - exp.size());

    Free(expLoc);

    PushInstructions(&exp, &total);
    PushInstruction(test, &total);
    PushInstructions(&code, &total);
    PushInstructions(&inc, &total);

    jump.SetA(-a);
    jump.SetB(0);

    FreeContext(&cont);
    PushInstruction(jump, &total);
    // std::cout << termcolor::green << "STACK: " << stack << std::endl;

    // Val ok ara fem
    int p = total.size();
    for(int i = 0; i < total.size(); i++){
        if(total[i].type == InstructionID::JUMP && total[i].temp == 1){
            // Val eh hem de posar el nombre de salts fin al final ja que això és un break
            total[i].temp = 0;
            total[i].SetA(total.size() - i - 1);
        }

        if(total[i].type == InstructionID::JUMP && total[i].temp == 2){
            total[i].temp = 0;

            total[i].SetA(total.size() - i - inc.size() - 3);
        }
    }

    PushInstructions(&total, to);
}

// TODO: Falta aillar contexto
void Assembler::AssembleWhile(Nodes::Node* whil, std::vector<Instruction> *to){
    std::vector<Instruction> exp, code, total;

    
    int cond = AssembleExpression(whil->children[0], &exp);
    
    std::vector<int> contLoc;
    
    // Aislar
    PushInstructions(&exp, &total);

    AssembleCode(whil->children[1], &code, &contLoc);

    FreeContext(&contLoc);

    int n = exp.size() + code.size() + 1;

    Instruction test(InstructionID::TEST, ParamType::AB);
    test.SetA(cond);
    test.SetB(code.size() + 2);

    PushInstruction(test, &total);

    Free(cond);
    PushInstructions(&code, &total);

    Instruction jump(InstructionID::JUMP, ParamType::AB);
    jump.SetA(-n - 1);

    PushInstruction(jump, &total);

    int p = total.size();
    for(int i = 0; i < total.size(); i++){
        if(total[i].type == InstructionID::JUMP && total[i].temp == 1){
            // Val eh hem de posar el nombre de salts fin al final ja que això és un break
            total[i].temp = 0;
            total[i].SetA(total.size() - i - 1);
        }

        if(total[i].type == InstructionID::JUMP && total[i].temp == 2){
            // Val eh ara aixo es un continue
            total[i].temp = 0;
            
            // total[i].SetA(-n -1 + i);
            total[i].SetA(total.size() - i - 2);
        }
    }

    PushInstructions(&total, to);
}

/*
void Assembler::AssembleReturn(Nodes::Node* ret, std::vector<Instruction> *to){
    AssembleExpression(ret->children[0], to);

    Instruction retu(InstructionID::RET, ParamType::A);
    retu.SetA(regPointer);

    PushInstruction(retu, to);
}
*/

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

int Assembler::AssembleDeclaration(Nodes::Node *dec, std::vector<Instruction> *to){
    // type DEC -> [EXP, TYPE]
    // Primer hauriem de fer un assemble de l'expressió

    // Suposo que hem de fer algo amb el type per optimitzar???
    int d;
    try {
        d = AssembleExpression(dec->children[0], to);
    } catch(std::string r){
        throw(r);
    }
    // Ara l'expression hauria d'estar al top del stack
    /*
    int n = GetNextFree();
    reserves[n].identifier = dec->GetDataString();
    reserves[n].isIdentifier = true;
    reserves[n].isFree = false;

    Instruction mov(InstructionID::MOVE, ParamType::AB);
    mov.SetA(d);
    mov.SetB(n);
    PushInstruction(mov, to);

    Free(d);
    */
    reserves[d].identifier = dec->GetDataString();
    reserves[d].isIdentifier = true;
    reserves[d].isFree = false;
    return d;
}

void Assembler::AssembleAssignation(Nodes::Node* assign, std::vector<Instruction> *to){
    // Ok primer l'expressió
    int where;
    try {
        where = GetRefId(assign->GetDataString());
    } catch(std::string ex){
        throw(ex);
    }
    
    int d = AssembleExpression(assign->children[0], to);

    Instruction move(InstructionID::MOVE, ParamType::AB);

    move.SetA(d);
    move.SetB(where);
    Free(d);

    PushInstruction(move, to);

}

int Assembler::AssembleExpression(Nodes::Node *exp, std::vector<Instruction> *to){
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

        Instruction op(TranslateBinOpId(exp->nd), ParamType::ABC);
        // Val si cap dels dos es valor podem cridar recursivament AssembleExpression amb un dels dos
        int storedA = AssembleExpression(f, to);
        op.SetA(storedA);
        // Ara tenim al nostre punter f assembleat. L'augmentem i assemblem t
        int storedB = AssembleExpression(s, to);
        op.SetB(storedB);

        // Val ara fem free i guardem
        int n = GetNextFree();
        op.SetC(n);
        Free(storedA);
        Free(storedB);

        PushInstruction(op, to);
        return n;
    }
    else if(exp->type == Nodes::NodeType::EXP_UNARY){
        Nodes::Node* f = exp->children[0];
        Instruction op(TranslateUnOpId(exp->nd), ParamType::AB);
        // Val doncs volem al nostre punter l'expressió
        int stored = AssembleExpression(f, to);
        // I ara li apliquem la operació
        op.SetA(stored);
        int n = GetNextFree();
        op.SetB(n);
        PushInstruction(op, to);
        return n;
    } else if(exp->IsValue()){
        // Bueno carreguem i ja està
        Instruction loadc(InstructionID::LOADC, ParamType::AB);

        int n = GetNextFree();
        loadc.SetA(n);
        loadc.SetB(GetConstId(exp));

        PushInstruction(loadc, to);
        return n;
    //} else if(exp->type == Nodes::NodeType::EXP_CALL){
        //return AssembleCall(exp, to);
    } else if(exp->type == Nodes::NodeType::REFERENCE){
        int ref;
        try {
            ref = GetRefId(exp->GetDataString());
        } catch(std::string ex){
            throw(ex);
        }

        // Val hem de moure a un lloc lliure
        int f = GetNextFree();

        Instruction mov(InstructionID::MOVE, ParamType::AB);
        mov.paramType = ParamType::AB;
        mov.SetA(ref);
        mov.SetB(f);
        PushInstruction(mov, to);

        return f;
    }
}

void Assembler::AssembleDef(Nodes::Node* def, std::vector<Instruction> *to){
    // El fill és una expression
    Instruction dif(InstructionID::DEF, ParamType::A);
    int d = AssembleExpression(def->children[0], to);
    dif.SetA(d);
    Free(d);

    PushInstruction(dif, to);
}

/*
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
*/

// De moment serà una resta chunga
InstructionID Assembler::TranslateBinOpId(int data){
    return (InstructionID) ((int) InstructionID::ADD + data - 5);
}

InstructionID Assembler::TranslateUnOpId(int data){
    return InstructionID::ADD;
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
                            // std::cout << "Si, son 2" << std::endl;
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

/*
int Assembler::GetRefId(std::string ref){
    for(int i = identifierStack.size() - 1; i >= 0; i--){
        if(identifierStack[i] == ref){
            // std::cout << "REF: " << ref << " " << i - stackPointer + 1 << " S: " << stackPointer <<  std::endl;
            return i - stacks.top();
        }
    }
    //std::cout << ref << std::endl;
    // Error
    throw(std::string("Name Error: " + ref + " is not defined."));
}
*/

int Assembler::GetRefId(std::string ref){
    for(int i = 0; i < reserves.size(); i++){
        if(reserves[i].isIdentifier && !reserves[i].isFree &&reserves[i].identifier == ref) return i;
    }
    std::cout << ref << " no hauria"<<  std::endl;
    return -1;
}

int Assembler::GetNextFree(){
    for(int i = 0; i < reserves.size(); i++){
        if(reserves[i].isFree){
            reserves[i].isFree = false;
            reserves[i].isIdentifier = false;
            return i;
        }
    }
    Reservation rev;
    reserves.push_back(rev);

    int p = reserves.size() - 1;
    reserves[p].isFree = 0;
    reserves[p].isIdentifier = 0;
    return p;
}

void Assembler::Free(int t){
    if(t >= reserves.size()) throw(std::string("No"));
    reserves[t].isFree = true;
}

void Assembler::FreeContext(std::vector<int> *v){
    for(int i = 0; i < v->size(); i++){
        Free((*v)[i]);
    }
}