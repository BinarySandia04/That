#include "reinterpreter.h"

#include <string>
#include <iostream>

#include "../vm/data.h"
#include "../compiler/nodes.h"

using namespace That;

std::string Reinterpreter::GetCode(Nodes::Node* node){
    std::cout << "Hola" << std::endl;
    return "#include <iostream>\nint main(){\n" + GetCodeBlock(node) + "\n}";
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
    return "";
}

std::string Reinterpreter::AssembleAssignation(Nodes::Node *n){
    return "";
}

std::string Reinterpreter::AssembleConditional(Nodes::Node *n){
    return "";
}

std::string Reinterpreter::AssembleWhile(Nodes::Node *n){
    return "";
}

std::string Reinterpreter::AssembleFor(Nodes::Node *n){
    return "";
}

std::string Reinterpreter::AssembleTempBreak(Nodes::Node *n){
    return "";
}

std::string Reinterpreter::AssembleTempSkip(Nodes::Node *n){
    return "";
}

std::string Reinterpreter::GetOperationGraphic(int op){
    Nodes::OpType oper = (Nodes::OpType) (op - 5);
    switch (oper)
    {
    case Nodes::OpType::ADD:
        return "+";
        break;
    case Nodes::OpType::SUBTRACT:
        return "-";
        break;
    case Nodes::OpType::MULT:
        return "*";
        break;
    case Nodes::OpType::DIV:
        return "/";
        break;
    case Nodes::OpType::MOD:
        return "%";
        break;
    default:
        break;
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