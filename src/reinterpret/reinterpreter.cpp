#include "reinterpreter.h"

#include <string>
#include <iostream>

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

        return '(' + storedA + '+' + storedB + ')';
    }
    else if(n->type == Nodes::NodeType::EXP_UNARY){
        Nodes::Node* f = n->children[0];
        std::string stored = AssembleExpression(f);
        return "-" + stored; // TODO: Acabar això
    } else if(n->IsValue()){
        return std::to_string(n->nd); // TODO: Esto
    } else if(n->type == Nodes::NodeType::REFERENCE){
        return n->GetDataString();
    }
}

std::string Reinterpreter::AssembleDef(Nodes::Node *n){
    return "std::cout << " + AssembleExpression(n->children[0]) + ";";
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
