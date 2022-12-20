#include "nodes.h"

#include <string>
#include <iostream>
#include <map>
#include <tuple>

using namespace Rux;

// Operands
Nodes::Literal   PlusOperand(Nodes::Literal first, Nodes::Literal second);
Nodes::Literal  MinusOperand(Nodes::Literal first, Nodes::Literal second);
Nodes::Literal  MultiOperand(Nodes::Literal first, Nodes::Literal second);
Nodes::Literal DivideOperand(Nodes::Literal first, Nodes::Literal second);

Nodes::Literal PlusOperandII(std::string first, std::string second);
Nodes::Literal PlusOperandRR(std::string first, std::string second);

Nodes::Literal MinusOperandII(std::string first, std::string second);
Nodes::Literal MinusOperandRR(std::string first, std::string second);

Nodes::Literal MultiOperandII(std::string first, std::string second);
Nodes::Literal MultiOperandRR(std::string first, std::string second);

Nodes::Literal DivideOperandRR(std::string first, std::string second);

// Mapa chingón
typedef Nodes::Literal (*binOpType)(std::string, std::string); 
std::map<
    std::tuple<
        Token::TokenType, Nodes::Literal::LiteralType, Nodes::Literal::LiteralType
    >, binOpType> binMap = {
        {{Token::TokenType::S_PLUS, Nodes::Literal::INT, Nodes::Literal::INT}, PlusOperandII},
        {{Token::TokenType::S_PLUS, Nodes::Literal::INT, Nodes::Literal::REAL}, PlusOperandRR},
        {{Token::TokenType::S_PLUS, Nodes::Literal::REAL, Nodes::Literal::INT}, PlusOperandRR},
        {{Token::TokenType::S_PLUS, Nodes::Literal::REAL, Nodes::Literal::REAL}, PlusOperandRR},

        {{Token::TokenType::S_SUBTRACT, Nodes::Literal::INT, Nodes::Literal::INT}, MinusOperandII},
        {{Token::TokenType::S_SUBTRACT, Nodes::Literal::INT, Nodes::Literal::REAL}, MinusOperandRR},
        {{Token::TokenType::S_SUBTRACT, Nodes::Literal::REAL, Nodes::Literal::INT}, MinusOperandRR},
        {{Token::TokenType::S_SUBTRACT, Nodes::Literal::REAL, Nodes::Literal::REAL}, MinusOperandRR},

        {{Token::TokenType::S_MULTIPLY, Nodes::Literal::INT, Nodes::Literal::INT}, MultiOperandII},
        {{Token::TokenType::S_MULTIPLY, Nodes::Literal::INT, Nodes::Literal::REAL}, MultiOperandRR},
        {{Token::TokenType::S_MULTIPLY, Nodes::Literal::REAL, Nodes::Literal::INT}, MultiOperandRR},
        {{Token::TokenType::S_MULTIPLY, Nodes::Literal::REAL, Nodes::Literal::REAL}, MultiOperandRR},
        
        {{Token::TokenType::S_DIVIDE, Nodes::Literal::INT, Nodes::Literal::INT}, DivideOperandRR},
        {{Token::TokenType::S_DIVIDE, Nodes::Literal::INT, Nodes::Literal::REAL}, DivideOperandRR},
        {{Token::TokenType::S_DIVIDE, Nodes::Literal::REAL, Nodes::Literal::INT}, DivideOperandRR},
        {{Token::TokenType::S_DIVIDE, Nodes::Literal::REAL, Nodes::Literal::REAL}, DivideOperandRR},
    };

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
    Nodes::Literal firstEval = Nodes::Literal("", Literal::LiteralType::VOID);
    Nodes::Literal secondEval = Nodes::Literal("", Literal::LiteralType::VOID);
    
    if(first != NULL) firstEval = first->Evaluate();
    if(second != NULL) secondEval = second->Evaluate();

    // std::cout << firstEval.GetValue() << " " << secondEval.GetValue() << std::endl;
    
    auto t = std::make_tuple(this->operation, firstEval.GetLiteralType(), secondEval.GetLiteralType());
    
    if(binMap.count(t)){
        return binMap[t](firstEval.GetValue(), secondEval.GetValue());
    }
    // Error no hi ha operació
}

Nodes::Literal Nodes::Call::Evaluate(){
    // std::cout << "Operado323232" << std::endl;
}

Nodes::Literal Nodes::Unary::Evaluate(){

    // std::cout << "Operado323232132132332" << std::endl;
}

Nodes::Literal PlusOperandII(std::string first, std::string second){
    int res;
    res = std::stoi(first) + std::stoi(second);
    return Nodes::Literal(std::to_string(res), Nodes::Literal::INT);
}

Nodes::Literal PlusOperandRR(std::string first, std::string second){
    double res;
    res = std::stod(first) + std::stod(second);
    return Nodes::Literal(std::to_string(res), Nodes::Literal::REAL);
}

Nodes::Literal MinusOperandII(std::string first, std::string second){
    int res;
    res = std::stoi(first) - std::stoi(second);
    return Nodes::Literal(std::to_string(res), Nodes::Literal::INT);
}

Nodes::Literal MinusOperandRR(std::string first, std::string second){
    double res;
    res = std::stod(first) - std::stod(second);
    return Nodes::Literal(std::to_string(res), Nodes::Literal::REAL);
}

Nodes::Literal MultiOperandII(std::string first, std::string second){
    int res;
    res = std::stoi(first) * std::stoi(second);
    return Nodes::Literal(std::to_string(res), Nodes::Literal::INT);
}

Nodes::Literal MultiOperandRR(std::string first, std::string second){
    double res;
    res = std::stod(first) * std::stod(second);
    return Nodes::Literal(std::to_string(res), Nodes::Literal::REAL);
}

Nodes::Literal DivideOperandRR(std::string first, std::string second){
    double res;
    res = std::stod(first) / std::stod(second);
    return Nodes::Literal(std::to_string(res), Nodes::Literal::REAL);
}