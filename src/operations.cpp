#include "nodes.h"

#include <string>
#include <iostream>

using namespace Rux;

// Operands
Nodes::Literal   PlusOperand(Nodes::Literal first, Nodes::Literal second);
Nodes::Literal  MinusOperand(Nodes::Literal first, Nodes::Literal second);
Nodes::Literal  MultiOperand(Nodes::Literal first, Nodes::Literal second);
Nodes::Literal DivideOperand(Nodes::Literal first, Nodes::Literal second);


Nodes::Literal Nodes::Expression::Evaluate(){
    // std::cout << this->expType << std::endl;
    if(this->expType == Nodes::ExpressionType::VALUE){
        // std::cout << "Queso " << this->value.GetValue() << std::endl;
        return this->value;
    }
    
    if(this->expType == Nodes::ExpressionType::UNARY){
        return dynamic_cast<Nodes::Unary*>(this)->Evaluate();
    }
}

Nodes::Literal Nodes::Binary::Evaluate(){
    // std::cout << "Operado" << std::endl;
    // std::cout << first->GetType() << " " << second->GetType() << std::endl;

    Nodes::Literal firstEval = Nodes::Literal("", Literal::LiteralType::VOID);
    Nodes::Literal secondEval = Nodes::Literal("", Literal::LiteralType::VOID);
    
    if(first != NULL) firstEval = first->Evaluate();
    if(second != NULL) secondEval = second->Evaluate();

    if(firstEval.GetLiteralType() > secondEval.GetLiteralType()){
        Nodes::Literal temp = secondEval;
        secondEval = firstEval;
        firstEval = temp;
    } 

    // std::cout << "a: " << firstEval.GetValue() << " b: " << secondEval.GetValue() << std::endl;

    // std::cout << "Hola?" << std::endl;

    switch (this->operation)
    {
    case Token::TokenType::S_PLUS:
        return PlusOperand(firstEval, secondEval);
    case Token::TokenType::S_SUBTRACT:
        return MinusOperand(firstEval, secondEval);
    case Token::TokenType::S_MULTIPLY:
        return MultiOperand(firstEval, secondEval);
    case Token::TokenType::S_DIVIDE:
        return DivideOperand(firstEval, secondEval);
    
    default:
        break;
    }
}

Nodes::Literal Nodes::Call::Evaluate(){
    // std::cout << "Operado323232" << std::endl;
}

Nodes::Literal Nodes::Unary::Evaluate(){

    // std::cout << "Operado323232132132332" << std::endl;
}

Nodes::Literal PlusOperand(Nodes::Literal first, Nodes::Literal second){
    Nodes::Literal::LiteralType f = first.GetLiteralType(), s = second.GetLiteralType();
    // std::cout << f << " " << s << std::endl;

    switch(f){
        case Nodes::Literal::INT:
            switch(s){
            case Nodes::Literal::INT:
                int i;
                i = std::stoi(first.GetValue()) + std::stoi(second.GetValue());
                return Nodes::Literal(std::to_string(i), Nodes::Literal::INT);
            case Nodes::Literal::REAL:
                double d;
                d = std::stod(first.GetValue()) + std::stod(second.GetValue());
                return Nodes::Literal(std::to_string(d), Nodes::Literal::REAL);
            default:
                break;
            }
        case Nodes::Literal::REAL:
            switch (s)
            {
            case Nodes::Literal::REAL:
                double d;
                d = std::stod(first.GetValue()) + std::stod(second.GetValue());
                return Nodes::Literal(std::to_string(d), Nodes::Literal::REAL);
                break;
            default:
                break;
            }
        default:
            // Throw error
            break;
        }
}

Nodes::Literal MinusOperand(Nodes::Literal first, Nodes::Literal second){
    Nodes::Literal::LiteralType f = first.GetLiteralType(), s = second.GetLiteralType();

    switch(f){
        case Nodes::Literal::INT:
            switch(s){
            case Nodes::Literal::INT:
                int i;
                i = std::stoi(first.GetValue()) - std::stoi(second.GetValue());
                return Nodes::Literal(std::to_string(i), Nodes::Literal::INT);
            case Nodes::Literal::REAL:
                double d;
                d = std::stod(first.GetValue()) - std::stod(second.GetValue());
                return Nodes::Literal(std::to_string(d), Nodes::Literal::REAL);
            default:
                break;
            }
        case Nodes::Literal::REAL:
            switch (s)
            {
            case Nodes::Literal::REAL:
                double d;
                d = std::stod(first.GetValue()) - std::stod(second.GetValue());
                return Nodes::Literal(std::to_string(d), Nodes::Literal::REAL);
                break;
            default:
                break;
            }
        default:
            // Throw error
            break;
        }
}

Nodes::Literal MultiOperand(Nodes::Literal first, Nodes::Literal second){
    Nodes::Literal::LiteralType f = first.GetLiteralType(), s = second.GetLiteralType();

    switch(f){
        case Nodes::Literal::INT:
            switch(s){
            case Nodes::Literal::INT:
                int i;
                i = std::stoi(first.GetValue()) * std::stoi(second.GetValue());
                return Nodes::Literal(std::to_string(i), Nodes::Literal::INT);
            case Nodes::Literal::REAL:
                double d;
                d = std::stod(first.GetValue()) * std::stod(second.GetValue());
                return Nodes::Literal(std::to_string(d), Nodes::Literal::REAL);
                break;
            default:
                break;
            }
        case Nodes::Literal::REAL:
            switch (s)
            {
            case Nodes::Literal::REAL:
                double d;
                d = std::stod(first.GetValue()) * std::stod(second.GetValue());
                return Nodes::Literal(std::to_string(d), Nodes::Literal::REAL);
                break;
            default:
                break;
            }
        default:
            // Throw error
            break;
        }
}

Nodes::Literal DivideOperand(Nodes::Literal first, Nodes::Literal second){
    Nodes::Literal::LiteralType f = first.GetLiteralType(), s = second.GetLiteralType();

    switch(f){
        case Nodes::Literal::INT:
            switch(s){
            case Nodes::Literal::INT:
                int i;
                i = std::stoi(first.GetValue()) / std::stoi(second.GetValue());
                return Nodes::Literal(std::to_string(i), Nodes::Literal::INT);
            case Nodes::Literal::REAL:
                double d;
                d = std::stod(first.GetValue()) / std::stod(second.GetValue());
                return Nodes::Literal(std::to_string(d), Nodes::Literal::REAL);
            default:
                break;
            }
        case Nodes::Literal::REAL:
            switch (s)
            {
            case Nodes::Literal::REAL:
                double d;
                d = std::stod(first.GetValue()) / std::stod(second.GetValue());
                return Nodes::Literal(std::to_string(d), Nodes::Literal::REAL);
                break;
            default:
                break;
            }
        default:
            // Throw error
            break;
        }
}