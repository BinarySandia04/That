#include "nodes.h"
#include <iostream>

using namespace That;


Nodes::Node::Node(NodeType type){
    this->type = type;
}

Nodes::Node::Node(){
    this->type = NodeType::NODE;
}

void Nodes::Node::Debug(){
    std::cout << "[ ";
    std::cout << "type: " << this->type << ", ";
    std::cout << "fills: ";
    for(int i = 0; i < this->children.size(); i++){
        this->children[i]->Debug();
    }
    std::cout << ", data: " << this->data.integer;
    std::cout << ", nd: " << this->nd;
    std::cout << " ]";
}