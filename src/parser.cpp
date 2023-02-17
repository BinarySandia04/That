#include "parser.h"

#include "headers/debug.hpp"

#include <iostream>
#include <vector>

using namespace That;

Parser::Parser(std::vector<Token> tokens){
    this->tokens = tokens;
}

Nodes::Node Parser::GenerateAST(){

    int currentEnd = 0;
    int currentStart = 0;
    int end = this->tokens.size();

    Nodes::Node root(Nodes::NodeType::NODE);

    Nodes::Node* nextNode;
    while(currentStart < end){
        
        // Calculem currentEnd
        for(currentEnd = currentStart; currentEnd < end && this->tokens[currentEnd].type != Token::SEMICOLON; currentEnd++);
        // Buscar expression
        // Podem veure a veure si es una declaració de funció
        if(IsType(this->tokens[currentStart].type)){
            // Vale ok podem ara llegir el nom de la variable i tal
            currentStart++;
            nextNode = new Nodes::Node(Nodes::NodeType::DECLARATION);

            Nodes::Node* expression;
            
            Token tok;
            if(!DigestName(Token::TokenType::IDENTIFIER, &tok, &currentStart)) break;
            nextNode->SetDataString(tok.value);
            if(!DigestName(Token::TokenType::A_ASSIGMENT, &tok, &currentStart)) break;

            // Calculem currentEnd
            for(currentEnd = currentStart; currentEnd < end && this->tokens[currentEnd].type != Token::SEMICOLON; currentEnd++);
            // Buscar expression
            GetExpression(&expression, currentStart, currentEnd - 1);
            nextNode->children.push_back(expression);
        } else {
            GetExpression(&nextNode, currentStart, currentEnd - 1);
        } 

        nextNode->Debug();
        // << std::endl << "Hola " << currentEnd << std::endl;
        root.children.push_back(nextNode);
        currentStart = currentEnd + 1;
    }

    // I ara aqui que tenim expressions podem generar codi suposo?
    

    // std::cout << "El tipo es: " << nextNode.GetType() << std::endl;


    std::cout << std::endl;
    // std::cout << nextNode.first->Evaluate().GetValue() << std::endl;

    return root;
}

void Parser::Eat(Token tok, Token comp, int *from){
    if(tok.type == comp.type){
        *from++;
    } else {
        // Call error, unexcepted identifier
        std::cout << "ERROR UNEXPECTED IDENTIFIER!!!" << std::endl;
    }
}

int Parser::EatParentesis(int from){
    Token::TokenType type = this->tokens[from].type;
    if(type != Token::TokenType::PARENTHESIS_OPEN) return from;
    // std::cout << "EIII" << std::endl;
    int j = 1;

    from++;
    while(j > 0){
        Token::TokenType type = this->tokens[from].type;
        if(type == Token::TokenType::PARENTHESIS_CLOSE) j--;
        if(type == Token::TokenType::PARENTHESIS_OPEN) j++;
        from++;
    }
    // std::cout << from << std::endl;
    return from-1;
}

void Parser::GetArguments(int from, int to, std::vector<Nodes::Node *>* parent){
    // std::cout << from << " " << to << std::endl;
    if(from > to) return;
    int a;
    Nodes::Node *arg;
    
    for(a = from; from <= to; from++){

        from = EatParentesis(from);
        if(this->tokens[from].type == Token::TokenType::COMMA){

            // std::cout << "a: " << a << " from: " << from << std::endl;
            GetExpression(&arg, a, from-1);
            parent->push_back(arg);
            a = from + 1;
        }
    }
    // std::cout << a << " " << from << std::endl;
    GetExpression(&arg, a, from-1);
    parent->push_back(arg);

}

void Parser::GetExpression(Nodes::Node** parent, int from, int to){
    // std::cout << "Expression from to: " << from << " " << to << std::endl;
    if(from == to){
        Token token = this->tokens[from];
        // Faltan literales, variables,
        if(token.IsLiteral()){
            // std::cout << "Detectado literal " << token.value << std::endl;
            Nodes::Node* lit = new Nodes::Node;
            switch(token.type){
                case Token::L_INT:
                    lit->type = Nodes::NodeType::VAL_INT;
                    lit->data.integer = std::stoi(token.value);
                    *parent = lit;
                    return;
                case Token::L_REAL:
                    lit->type = Nodes::NodeType::VAL_REAL;
                    lit->data.real = std::stod(token.value);
                    *parent = lit;
                    return;
                case Token::L_STRING:
                    lit->type = Nodes::NodeType::VAL_STRING;
                    lit->nd = token.value.size();
                    lit->data.bytes = new char[lit->nd];
                    for(int i = 0; i < lit->nd; i++){
                        lit->data.bytes[i] = token.value[i];
                    }
                    *parent = lit;
                    return;
                case Token::L_TRUE:
                    lit->type = Nodes::NodeType::VAL_BOOLEAN;
                    lit->data.integer = 1;
                    *parent = lit;
                    return;
                case Token::L_FALSE:
                    lit->type = Nodes::NodeType::VAL_BOOLEAN;
                    lit->data.integer = 0;
                    *parent = lit;
                    return;
                case Token::L_NULL:
                    lit->type == Nodes::NodeType::VAL_NULL;
                    *parent = lit;
                    return;
                default:
                    break;
            }
        }
        // Vale comprovem s es una call
        
    }

    // Comprovem si es una call
    if(this->tokens[from].IsIdentifier()){
        // Veure si es funció
        if(from == to){
            Nodes::Node *id = new Nodes::Node(Nodes::NodeType::REFERENCE);
            id->SetDataString(this->tokens[from].value);
            *parent = id;
            return;
        } else {
            int ffrom = from + 1;

            Token::TokenType type = this->tokens[ffrom].type;
            int j = 0;
            if(type == Token::PARENTHESIS_OPEN){
                j = 1;
                // std::cout << "Hola si???";
                // std::cout << "Parentesis " << std::endl;
                for(ffrom++; ffrom <= to; ffrom++){
                    type = this->tokens[ffrom].type;
                    if(type == Token::TokenType::PARENTHESIS_OPEN) j++;
                    if(type == Token::TokenType::PARENTHESIS_CLOSE) j--;

                    if(j == 0){
                        // std::cout << "Aqui hem arribat" << std::endl;
                        if(ffrom == to){
                            // Hacer cosas
                            Nodes::Node *call = new Nodes::Node(Nodes::NodeType::EXP_CALL);
                            call->SetDataString(this->tokens[from].value);
                            *parent = call;
                            ffrom = from + 2;
                            // std::cout << "ffrom: " << ffrom << " to: " << to - 1 << std::endl;
                            // Aqui hay que pillar argumentos
                            // std::cout << ffrom + 1 << " " << to - 1 << std::endl;
                            GetArguments(ffrom, to - 1, &(call->children));
                            return;
                        }
                        break;
                    }
                }

                
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
            Nodes::Node *bin = new Nodes::Node(Nodes::NodeType::EXP_BINARY);
            Nodes::Node *first = NULL, *second = NULL;

            bin->nd = (int) type -  (int) Token::S_PLUS;
            
            GetExpression(&first, from, i - 1);
            GetExpression(&second, i + 1, to);
            
            // std::cout << "Hola!!" << std::endl;
            // std::cout << first << " " << second << std::endl;
            bin->children.push_back(first);
            bin->children.push_back(second);
            *parent = bin;

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

        if((int) type >= Token::S_PLUS && type <= Token::S_MODULO){
            Nodes::Node *bin = new Nodes::Node(Nodes::EXP_BINARY);
            bin->nd = (int) type -  (int) Token::S_PLUS;

            Nodes::Node *first = NULL, *second = NULL;
            GetExpression(&first, from, i - 1);
            GetExpression(&second, i + 1, to);

            
            // std::cout << "AADIOS!!" << std::endl;
            // std::cout << first << " " << second << std::endl;
            bin->children.push_back(first);
            bin->children.push_back(second);
            *parent = bin;

            return;
        }
    }
}

bool Parser::IsType(Token::TokenType type){
    return (
    type == Token::TokenType::T_INT || 
    type == Token::TokenType::T_REAL || 
    type == Token::TokenType::T_BOOLEAN || 
    type == Token::TokenType::T_STRING);
}

bool Parser::DigestName(Token::TokenType expected, Token *dir, int *index){
    if(this->tokens[*index ].type == expected){
        *dir = this->tokens[*index];
        *index = *index + 1;
        return true;
    } else return false;
}