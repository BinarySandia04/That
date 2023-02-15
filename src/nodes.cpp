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
    std::cout << "Hola!" << std::endl;
}