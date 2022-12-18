#include "nodes.h"
#include <string>
#include <iostream>

using namespace Glass;

Nodes::Literal Nodes::Expression::Evaluate(){
    std::cout << this->expType << std::endl;
    if(this->expType == Nodes::ExpressionType::VALUE){
        std::cout << "Queso " << this->value.GetValue() << std::endl;
        return this->value;
    }
    
    if(this->expType == Nodes::ExpressionType::UNARY){
        return dynamic_cast<Nodes::Unary*>(this)->Evaluate();
    }
}

Nodes::Literal Nodes::Binary::Evaluate(){
    std::cout << "Operado" << std::endl;
    std::cout << first->GetType() << " " << second->GetType() << std::endl;

    Nodes::Literal firstEval = first->Evaluate();
    Nodes::Literal secondEval = second->Evaluate();

    std::cout << "a: " << firstEval.GetValue() << " b: " << secondEval.GetValue() << std::endl;

    std::cout << "Hola?" << std::endl;

    switch (this->operation)
    {
    case Token::TokenType::S_PLUS:
        switch(firstEval.GetLiteralType()){
        case Nodes::Literal::INT:
            switch(secondEval.GetLiteralType()){
            case Nodes::Literal::INT:
                int res;
                res = std::stoi(firstEval.GetValue()) + std::stoi(secondEval.GetValue());
                std::cout << res <<std::endl;
                return Nodes::Literal(std::to_string(res), Nodes::Literal::INT);
            default:
                break;
            }
        default:
            // Throw error
            break;
        }
        break;
    case Token::TokenType::S_SUBTRACT:
        switch(firstEval.GetLiteralType()){
        case Nodes::Literal::INT:
            switch(secondEval.GetLiteralType()){
            case Nodes::Literal::INT:
                int res;
                res = std::stoi(firstEval.GetValue()) - std::stoi(secondEval.GetValue());
                std::cout << res <<std::endl;
                return Nodes::Literal(std::to_string(res), Nodes::Literal::INT);
            default:
                break;
            }
        default:
            // Throw error
            break;
        }
        break;
    case Token::TokenType::S_MULTIPLY:
        switch(firstEval.GetLiteralType()){
        case Nodes::Literal::INT:
            switch(secondEval.GetLiteralType()){
            case Nodes::Literal::INT:
                int res;
                res = std::stoi(firstEval.GetValue()) * std::stoi(secondEval.GetValue());
                std::cout << res <<std::endl;
                return Nodes::Literal(std::to_string(res), Nodes::Literal::INT);
            default:
                break;
            }
        default:
            // Throw error
            break;
        }
        break;
    case Token::TokenType::S_DIVIDE:
        switch(firstEval.GetLiteralType()){
        case Nodes::Literal::INT:
            switch(secondEval.GetLiteralType()){
            case Nodes::Literal::INT:
                int res;
                res = std::stoi(firstEval.GetValue()) / std::stoi(secondEval.GetValue());
                std::cout << res <<std::endl;
                return Nodes::Literal(std::to_string(res), Nodes::Literal::INT);
            default:
                break;
            }
        default:
            // Throw error
            break;
        }
        break;
    
    default:
        break;
    }
}

Nodes::Literal Nodes::Call::Evaluate(){
    std::cout << "Operado323232" << std::endl;
}

Nodes::Literal Nodes::Unary::Evaluate(){

    std::cout << "Operado323232132132332" << std::endl;
}