#include "parser.h"

#include "headers/debug.hpp"

#include <iostream>
#include <vector>

using namespace That;

Parser::Parser(std::vector<Token> tokens){
    this->tokens = tokens;
}

Nodes::Node Parser::GenerateAST(){

    int current = 0;
    int start;
    int end;

    Nodes::Node root(Nodes::NodeType::NODE);


    for(start = current, end = current; end < this->tokens.size(); end++){
        if(this->tokens[end].type == Token::SEMICOLON) break;
    }
    current = end + 1;

    Nodes::Node* nextNode;
    GetExpression(&nextNode, start, end - 1);

    // I ara aqui que tenim expressions podem generar codi suposo?
    

    // std::cout << "El tipo es: " << nextNode.GetType() << std::endl;

    nextNode->Debug();

    std::cout << std::endl;
    // std::cout << nextNode.first->Evaluate().GetValue() << std::endl;

    return root;
}

void Parser::Eat(Token tok, Token comp, int *from){
    if(tok.type == comp.type){
        *from++;
    } else {
        // Call error, unexcepted identifier
        std::cout << "ERROR HOY NO HE COMIDO!!!" << std::endl;
    }
}

int Parser::EatParentesis(int from){
    Token::TokenType type = this->tokens[from].type;
    if(type != Token::TokenType::PARENTHESIS_OPEN) return from;
    int j = 1;

    from++;
    while(j > 0){
        Token::TokenType type = this->tokens[from].type;
        if(type == Token::TokenType::PARENTHESIS_CLOSE) j++;
        if(type == Token::TokenType::PARENTHESIS_OPEN) j--;
        from++;
    }
    std::cout << from << std::endl;
    return from;
}

void Parser::GetArguments(int from, int to, std::vector<Nodes::Node *>* parent){
    // std::cout << from << " " << to << std::endl;
    if(from == to) return;
    for(int a = from; from <= to; from++){
        from = EatParentesis(from);
        if(this->tokens[from].type == Token::TokenType::COMMA || from >= to){

            Nodes::Node *arg;
            // std::cout << "a: " << a << " from: " << from << std::endl;
            GetExpression(&arg, a, from-1);
            parent->push_back(arg);
            a = from+1;
        }
    }
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
                        if(ffrom == to){
                            // Hacer cosas
                            Nodes::Node *call = new Nodes::Node(Nodes::NodeType::EXP_CALL);
                            call->SetDataString(this->tokens[from].value);
                            *parent = call;
                            ffrom = from + 1;

                            // Aqui hay que pillar argumentos
                            std::cout << ffrom + 1 << " " << to - 1 << std::endl;
                            GetArguments(ffrom + 1, to, &(call->children));
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