#include "reinterpreter.h"

#include <string>
#include <iostream>

#include "../vm/data.h"
#include "../compiler/nodes.h"

using namespace That;

std::string Reinterpreter::GetCode(Nodes::Node* node){
    std::cout << "Hola" << std::endl;
    return "#include <bits/stdc++.h>\nint main(){" + GetCodeBlock(node) + "}";
}

std::string Reinterpreter::GetCodeBlock(Nodes::Node *node){
    std::string res = "";
    for(int i = 0; i < node->children.size(); i++){
        Nodes::Node* n = node->children[i];
        Nodes::NodeType t = n->type;
        
        try {
            if(n->IsExpression()) res += AssembleExpression(n);
            else if(t == Nodes::DEF_FUNCTION) res += AssembleDef(n);
            else if(t == Nodes::DECLARATION) res += AssembleDeclaration(n);
            else if(t == Nodes::ASSIGNATION) res += AssembleAssignation(n);
            // else if(t == Nodes::RETURN) AssembleReturn(n, to);
            else if(t == Nodes::IF) res += AssembleConditional(n);
            else if(t == Nodes::WHILE) res += AssembleWhile(n);
            else if(t == Nodes::FOR) res += AssembleFor(n);
            else if(t == Nodes::BREAK) res += AssembleTempBreak(n);
            else if(t == Nodes::SKIP) res += AssembleTempSkip(n);
        } catch(std::string r){
            throw(r);
        }   
    }
    return res;
}

std::string Reinterpreter::AssembleExpression(Nodes::Node *n){
    if(n->type == Nodes::NodeType::EXP_BINARY){
        Nodes::Node* f = n->children[0], *s = n->children[1], *t;

        std::string storedA = AssembleExpression(f);
        std::string storedB = AssembleExpression(s);

        return '(' + storedA + GetOperationGraphic((int) n->nd) + storedB + ')';
    }
    else if(n->type == Nodes::NodeType::EXP_UNARY){
        Nodes::Node* f = n->children[0];
        std::string stored = AssembleExpression(f);
        return GetOperationGraphic((int) n->nd) + stored; // TODO: Acabar això
    } else if(n->IsValue()){
        return GetLiteralValue(n);
    } else if(n->type == Nodes::NodeType::REFERENCE){
        return n->GetDataString();
    }
}

std::string Reinterpreter::AssembleDef(Nodes::Node *n){
    return "std::cout << " + AssembleExpression(n->children[0]) + " << std::endl;";
}

std::string Reinterpreter::AssembleDeclaration(Nodes::Node *n){
        // type DEC -> [EXP, TYPE]
    // Primer hauriem de fer un assemble de l'expressió

    // Suposo que hem de fer algo amb el type per optimitzar???
    std::string d;
    d = AssembleExpression(n->children[0]);
    std::string t = GetType(n->children[1]);
    std::string name = n->GetDataString();
    return t + " " + name + " = " + d + ";";
}

std::string Reinterpreter::AssembleAssignation(Nodes::Node *n){
    // Ok primer l'expressió
    std::string s = n->GetDataString();
    
    std::string e = AssembleExpression(n->children[0]);
    return s + " = " + e + ";";
}

std::string Reinterpreter::AssembleConditional(Nodes::Node *n){

    std::string fin = "";
    fin += "if(";
    for(int i = 0; i < n->children.size(); i++){
        if(i % 2 == 0){ // Ultimo codigo o condicion
            if(n->children.size() % 2 == 1 && i == n->children.size() - 1){
                fin += GetCodeBlock(n->children[i]) += "}";
            } else {
                fin += AssembleExpression(n->children[i]) + "){";
            }
        } else { // Codigo
            fin += GetCodeBlock(n->children[i]) + "}";

            if(n->children.size() % 2 == 1 && i == n->children.size() - 2){
                fin += "else {";
            } else if(i <= n->children.size() - 2) {
                fin += "else if(";
            }
        }
    }
    return fin;
}

std::string Reinterpreter::AssembleWhile(Nodes::Node *n){
    std::string cond = AssembleExpression(n->children[0]);
    std::string code = GetCodeBlock(n->children[1]);

    return "while(" + cond + "){" + code + "}";
}

std::string Reinterpreter::AssembleFor(Nodes::Node *n){
    std::string init = GetCodeBlock(n->children[0]);
    std::string expLoc = AssembleExpression(n->children[1]);
    std::string inc = GetCodeBlock(n->children[2]);
    
    std::string code = GetCodeBlock(n->children[3]);
    if(inc.size() > 0) inc.pop_back();
    return "for(" + init + expLoc + ";" + inc + "){" + code + "}";
}

std::string Reinterpreter::AssembleTempBreak(Nodes::Node *n){
    return "";
}

std::string Reinterpreter::AssembleTempSkip(Nodes::Node *n){
    return "";
}

std::string Reinterpreter::GetOperationGraphic(int op){
    OpType oper = (OpType) op;
    switch (oper)
    {
    case OpType::OP_ADD:
        return "+";
        break;
    case OpType::OP_SUBTRACT:
        return "-";
        break;
    case OpType::OP_MUL:
        return "*";
        break;
    case OpType::OP_DIV:
        return "/";
        break;
    case OpType::OP_MOD:
        return "%";
        break;
    case OpType::OP_AND:
        return "&&";
        break;
    case OpType::OP_OR:
        return "||";
        break;
    case OpType::OP_NOT:
        return "!";
        break;
    case OpType::OP_LT:
        return "<";
        break;
    case OpType::OP_GT:
        return ">";
        break;
    case OpType::OP_LEQ:
        return "<=";
        break;
    case OpType::OP_GEQ:
        return ">=";
        break;
    case OpType::OP_EQ:
        return "==";
        break;
    default:
        break;
    }
}

std::string Reinterpreter::GetType(Nodes::Node *type){
    std::string s;
    switch((Type) type->nd){
        case Type::INT:
            return "int";
        case Type::REAL:
            return "double";
        case Type::STRING:
            return "std::string";
        case Type::BOOL:
            return "bool";
        default:
            return "auto";
    }
}

std::string Reinterpreter::GetLiteralValue(Nodes::Node *val){
    std::string s;
    int i = 0;
    switch (val->type)
    {
    case Nodes::VAL_INT:
        return std::to_string(val->data.integer);
        break;
    case Nodes::VAL_NULL:
        return "NULL";
        break;
    case Nodes::VAL_BOOLEAN:
        if(val->nd) return "true";
        return "false";
        break;
    case Nodes::VAL_STRING:
        s = "\"";
        for(i = 0; i < val->nd; i++){
            s += (char) val->data.bytes[i];
        }
        s += "\"";
        return s;
        break;
    default:
        break;
    }
    return std::to_string(val->data.integer); // TODO: Esto
}