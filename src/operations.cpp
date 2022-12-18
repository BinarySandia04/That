#include "nodes.h"
#include <string>
#include <iostream>

using namespace Glass;

Nodes::Literal Nodes::Expression::Evaluate(){
    std::cout << this->expType << std::endl;
    if(this->expType == Nodes::ExpressionType::VALUE){
        return first->value;
    }

    if(this->expType == Nodes::ExpressionType::BINARY){
        return reinterpret_cast<Nodes::Binary*>(first)->Evaluate();
    }
    
    if(this->expType == Nodes::ExpressionType::UNARY){
        return reinterpret_cast<Nodes::Unary*>(first)->Evaluate();
    }

    if(this->expType == Nodes::ExpressionType::VALUE){
        return reinterpret_cast<Nodes::Literal*>(first)->Evaluate();
    }
}

Nodes::Literal Nodes::Binary::Evaluate(){
    std::cout << "Operado" << std::endl;
    std::cout << first->GetType() << " " << second->GetType() << std::endl;
    std::cout << Nodes::Literal::INT << std::endl;

    Nodes::Literal firstEval = first->Evaluate();
    Nodes::Literal secondEval = second->Evaluate();

    std::cout << "Hola?" << std::endl;

    switch(firstEval.GetLiteralType()){
        case Nodes::Literal::INT:
            switch(secondEval.GetLiteralType()){
                case Nodes::Literal::INT:
                    int res;
                    res = std::stoi(firstEval.GetValue()) + std::stoi(secondEval.GetValue());
                    std::cout << res <<std::endl;
                    return Nodes::Literal(res + "", Nodes::Literal::INT);
                default:
                    break;
            }
        default:
            // Throw error
            break;
    }
}

Nodes::Literal Nodes::Literal::Evaluate(){
    return *this;
}

Nodes::Literal Nodes::Call::Evaluate(){

}

Nodes::Literal Nodes::Unary::Evaluate(){

}