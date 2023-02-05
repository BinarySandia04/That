#include "parser.h"

#include <iostream>
#include <vector>

using namespace Radic;

void test(Nodes::Expression *node){
    // std::cout << "Tipo: " << node->GetType() << std::endl;
}


Parser::Parser(std::vector<Token> tokens){
    this->tokens = tokens;
}

Nodes::Node Parser::GenerateAST(){
    Nodes::Node root(Nodes::NodeType::NODE);

    int current = 0;
    int start;
    int end;

    for(start = current, end = current; end < this->tokens.size(); end++){
        if(this->tokens[end].type == Token::SEMICOLON) break;
    }
    current = end + 1;

    Nodes::Expression nextNode;
    GetExpression(&nextNode.first, start, end - 1);

    // std::cout << "El tipo es: " << nextNode.GetType() << std::endl;

    std::cout << nextNode.first->Evaluate().GetValue() << std::endl;

    return root;
}

void Parser::GetExpression(Nodes::Expression** parent, int from, int to){
  
    /*
    for(int i = from; i <= to; i++){
        std::cout << "[";
        std::cout << "type: " << tokens[i].type;
        if(tokens[i].value.size() > 0){
            std::cout << ", value: " << tokens[i].value << "]";
        } else {
            std::cout << "]";
        }
        if(i < tokens.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    */

    if(from == to){
        Token token = this->tokens[from];
        // Faltan literales, variables,
        if(token.IsLiteral()){
            // std::cout << "Detectado literal " << token.value << std::endl;

            if(token.type == Token::L_INT){
                // std::cout << "Value: " << token.value << std::endl;
                Nodes::Literal lit(token.value, Nodes::Literal::LiteralType::INT);
                Nodes::Expression *par = new Nodes::Expression(lit);
                *parent = par;
                return;
            }

            if(token.type == Token::L_REAL){
                Nodes::Literal lit(token.value, Nodes::Literal::LiteralType::REAL);
                Nodes::Expression *par = new Nodes::Expression(lit);
                *parent = par;
                return;
            }

            if(token.type == Token::L_STRING){
                Nodes::Literal lit(token.value, Nodes::Literal::LiteralType::STRING);
                Nodes::Expression *par = new Nodes::Expression(lit);
                *parent = par;
                return;
            }

            if(token.type == Token::L_TRUE){
                Nodes::Literal lit("t", Nodes::Literal::LiteralType::BOOLEAN);
                Nodes::Expression *par = new Nodes::Expression(lit);
                *parent = par;
                return;
            }
            
            if(token.type == Token::L_FALSE){
                Nodes::Literal lit("f", Nodes::Literal::LiteralType::BOOLEAN);
                Nodes::Expression *par = new Nodes::Expression(lit);
                *parent = par;
                return;
            }

            if(token.type == Token::L_NULL){
                Nodes::Literal lit;
                Nodes::Expression *par = new Nodes::Expression(lit);
                *parent = par;
                return;
            }
        }
    }

    if(this->tokens[to].type == Token::PARENTHESIS_CLOSE){
        // std::cout << "Hola" << std::endl;
        int l = 0;
        int valid = 1;
        for(int j = to; j >= from; j--){
            if(this->tokens[j].type == Token::PARENTHESIS_OPEN) l--;
            if(this->tokens[j].type == Token::PARENTHESIS_CLOSE) l++;

            // std::cout << "l: " << l << " " << this->tokens[j].type << std::endl;
            if(l < 1 && j != from){
                valid = 0;
                break;
            }
        }

        if(l == 0 && valid && this->tokens[from].type == Token::PARENTHESIS_OPEN){
            // std::cout << "Quitando parentesis" << std::endl;
            GetExpression(parent, from + 1, to - 1);
            return;
        }

        if(!valid){
            std::cout << "error?" << std::endl;
        }
    }

    int i, j;
    // std::cout << "SI funcion" << std::endl;
    // +, - 
    for(i = to; i >= from; i--){
        Token::TokenType type = this->tokens[i].type;
        if(type == Token::PARENTHESIS_CLOSE){
            j = 1;
            // std::cout << "Parentesis " << std::endl;
            for(i--; j != 0; i--){
                type = this->tokens[i].type;
                if(type == Token::TokenType::PARENTHESIS_CLOSE) j++;
                if(type == Token::TokenType::PARENTHESIS_OPEN) j--;
            }
            i++;
            continue;
        }

        if(type == Token::S_PLUS || type == Token::S_SUBTRACT){
            Nodes::Expression *first = NULL, *second = NULL;
            
            GetExpression(&first, from, i - 1);
            GetExpression(&second, i + 1, to);
            
            // std::cout << "Hola!!" << std::endl;
            // std::cout << first << " " << second << std::endl;

            Nodes::Binary *bin = new Nodes::Binary(first, this->tokens[i].type, second);
            *parent = reinterpret_cast<Nodes::Expression*>(bin);

            return;
        }
    }

    for(i = to; i >= from; i--){
        Token::TokenType type = this->tokens[i].type;
        if(type == Token::PARENTHESIS_CLOSE){
            j = 1;
            // std::cout << "Parentesis " << std::endl;
            for(i--; j > 0; i--){
                type = this->tokens[i].type;
                if(type == Token::TokenType::PARENTHESIS_CLOSE) j++;
                if(type == Token::TokenType::PARENTHESIS_OPEN) j--;
            }
            i++;
            continue;
        }

        if(type == Token::S_MULTIPLY || type == Token::S_DIVIDE 
        || type == Token::S_INTDIVIDE || type == Token::S_MODULO){
            Nodes::Expression *first = NULL, *second = NULL;
            GetExpression(&first, from, i - 1);
            GetExpression(&second, i + 1, to);

            
            // std::cout << "AADIOS!!" << std::endl;
            // std::cout << first << " " << second << std::endl;

            Nodes::Binary *bin = new Nodes::Binary(first, this->tokens[i].type, second);
            *parent = reinterpret_cast<Nodes::Expression*>(bin);

            return;
        }
    }
}