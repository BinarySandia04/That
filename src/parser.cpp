#include "parser.h"

#include "headers/debug.hpp"

#include <iostream>
#include <vector>

using namespace That;

Parser::Parser(std::vector<Token> tokens){
    this->tokens = tokens;
}


Nodes::Node Parser::GenerateAST(){
    Nodes::Node root(Nodes::NodeType::NODE);

    GenerateCode(0, this->tokens.size(), &root);

    root.Debug();
    std::cout << std::endl;

    return root;
}

void Parser::GenerateCode(int from, int to, Nodes::Node *parent){
    Nodes::Node *next;
    int currentEnd;
    while(from < to){
        // Val aqui anem a fer check de una funció!
        int nF = from;

        GetCodeFunction(&next, from, &nF);
        if(nF != from) { parent->children.push_back(next); from = nF; continue; }

        GetCodeConditional(&next, from, &nF);
        if(nF != from) { parent->children.push_back(next); from = nF; continue; }

        GetCodeWhile(&next, from, &nF);
        if(nF != from) { parent->children.push_back(next); from = nF; continue; }

        currentEnd = GetNext(from, to, Token::SEMICOLON);
        GetCodeLine(parent, from, currentEnd);
    
        from = currentEnd + 1;

    }
}

/*
if(<condició>){
    // Codi
} else if(<condició>) {
    // Codi
} else {
    // Codi
}
*/
void Parser::GetCodeConditional(Nodes::Node **root, int from, int *end){
    if(!Eat(this->tokens[from].type, Token::TokenType::K_IF, &from)) return;

    Nodes::Node *theIf = new Nodes::Node(Nodes::NodeType::IF);

    GetConditional(from, &from, theIf);
    // Val aixo ens deixa allà
    // std::cout << "F: " << from << std::endl;
    // theIf->Debug();

    while(Eat(this->tokens[from].type, Token::TokenType::K_ELSE, &from)){
        if(Eat(this->tokens[from].type, Token::TokenType::K_IF, &from)){
            GetConditional(from, &from, theIf);
            theIf->nd += 1;
            continue;
        } else if(Eat(this->tokens[from].type, Token::TokenType::CURLY_BRACKET_OPEN, &from)) {
            // Estem al else, llegim codi i fem push!!!
            
            int to = GetNext(from, -1, Token::TokenType::CURLY_BRACKET_CLOSE);
            Nodes::Node *elseCode = new Nodes::Node(Nodes::NodeType::NODE);
            GenerateCode(from, to-1, elseCode);
            theIf->children.push_back(elseCode);
            from = to + 1;
        }
        break;
    }

    
    *root = theIf;
    *end = from;
}

// Simplement es
/*
while (expressió) {
    // codi
}
*/
void Parser::GetCodeWhile(Nodes::Node **root, int from, int *end){
    if(!Eat(this->tokens[from].type, Token::TokenType::K_WHILE, &from)) return;

    Nodes::Node *bucle = new Nodes::Node(Nodes::NodeType::WHILE);
    GetConditional(from, end, bucle);

    *root = bucle;
}

/*
Aconsegueix parsejar una cosa de la forma
(expressió) {
    // Codi
}
*/
void Parser::GetConditional(int from, int* end, Nodes::Node* pushNode){
    // Aconseguim la condició
    Nodes::Node *content = new Nodes::Node(Nodes::NodeType::NODE);

    int to = GetNext(from, -1, Token::TokenType::CURLY_BRACKET_OPEN);

    // std::cout << "EXP: " << from << " " << to - 1 << std::endl;
    
    Nodes::Node *expression;
    GetExpression(from, to-1, &expression);

    // std::cout << expression->type << std::endl;

    // I el contingut del if
    from = to + 1;
    to = GetNext(from, -1, Token::TokenType::CURLY_BRACKET_CLOSE);
    
    GenerateCode(from, to-1, content);

    pushNode->children.push_back(expression);
    pushNode->children.push_back(content);

    *end = to + 1;
}

/*
Volem un node de la forma
func nd: nombre de params
1: Identificador de la funció
2: Tipus que retorna la funció
3: Codi de dins
4 <-> 4+nd: Paràmetres que rep

-1-
func <def> : (<type def1, ..., type defn>) => type {
    <AST>
} 

Val estaria guai fer que es pugui escriure també de forma

-2-
func <def> { // Sense arguments i no retorna res
    <AST>
}

-3-
func <def> : (<type def1, ..., type defn>) { // Amb arguments i no retorna res
    <AST>
}

-4-
func <def> => type { // Sense arguments retorna el tipus
    <AST>
}
*/
void Parser::GetCodeFunction(Nodes::Node **root, int from, int *end){
    
    if(!Eat(this->tokens[from].type, Token::TokenType::FUNCTION_DECLARATION, &from)) return;

    Nodes::Node* function = new Nodes::Node(Nodes::NodeType::FUNCTION);

    // A partir d'aqui són excepcions
    if(!Eat(this->tokens[from].type, Token::TokenType::IDENTIFIER, &from)) return;
    
    from--;
    std::string funcIdentifier = this->tokens[from].value;
    from++;
    
    std::cout << "id: " << funcIdentifier << std::endl;
    
    std::vector<Nodes::Node*> parameters;
    Nodes::Node *code = new Nodes::Node(), 
                *type = new Nodes::Node(Nodes::NodeType::TYPE),
                *identifier = new Nodes::Node(Nodes::NodeType::REFERENCE);

    int to;
    if(Eat(this->tokens[from].type, Token::TokenType::TWO_POINTS, &from)){
        // Tipus 1, 3 - Tenim paràmetres! Ara falta veure si hi ha return type
        if(!Eat(this->tokens[from].type, Token::TokenType::PARENTHESIS_OPEN, &from)) return;
        // Val ara estem apuntant al primer parèntesis i ara agafarem les coses

        function->nd = parameters.size();

        to = GetNext(from, -1, Token::TokenType::PARENTHESIS_CLOSE);
        GetFunctionParameters(from, to-1, &parameters); // Lo de dins sense parèntesis
        
        from = to+1;
    }

    if(Eat(this->tokens[from].type, Token::TokenType::ARROW, &from)){
        // Ara tenim return type! Anem a llegir-lo!
        if(!IsType(this->tokens[from].type)) return;
        int typeId = (int) this->tokens[from].type;
        std::cout << "typeID: " << typeId << std::endl;
        
        type->nd = typeId;
        from++;
    } else {
        type->nd = -1; // Suposo que això és void
    }

    
    if(!Eat(this->tokens[from].type, Token::TokenType::CURLY_BRACKET_OPEN, &from)) return; // Aqui si es error. No ha posat { el senyor

    to = GetNext(from, -1, Token::TokenType::CURLY_BRACKET_CLOSE);

    // Empenyem info bàsica
    identifier->SetDataString(funcIdentifier);
    function->children.push_back(identifier);
    function->children.push_back(type);
    
    // From apunta a '{', to apunta a '}', generem el codi
    GenerateCode(from, to-1, code);

    // L'afegim
    function->children.push_back(code);

    // I els parametres
    for(int i = 0; i < parameters.size(); i++){
        function->children.push_back(parameters[i]);
    }

    *root = function;
    *end = to+1;
}

// int a, string b, real c
void Parser::GetFunctionParameters(int from, int to, std::vector<Nodes::Node *>* container){
    if(from > to) return;

    Nodes::Node *next;
    int tA = from;

    do {
        tA = GetNext(from, to+1, Token::TokenType::COMMA);

        GetFunctionParameter(from, tA-1, &next);
        container->push_back(next);

        from = tA+1;
    } while(from < to || tA < to);
}

// int a
void Parser::GetFunctionParameter(int from, int to, Nodes::Node **writeNode){
    // En principi to - from = 1
    Nodes::Node *param = new Nodes::Node(Nodes::NodeType::PARAMETER), *typeNode = new Nodes::Node(Nodes::NodeType::TYPE);
    param->SetDataString(this->tokens[to].value);
    typeNode->nd = (int) this->tokens[from].type;
    param->children.push_back(typeNode);

    *writeNode = param;
}

// Aconseguim una linea de codi de la forma
// <tipus> <var> = exp
// On var es opcional, pero si es posa cal el "="
// i tipus és opcional en cas que l'expressió sigui <var> = exp
void Parser::GetCodeLine(Nodes::Node *root, int from, int to){

    if(IsType(this->tokens[from].type)){
        // Aqui podriem optimitzar memòria
        Nodes::Node *typeNode = new Nodes::Node(Nodes::NodeType::TYPE);
        typeNode->nd = (int) this->tokens[from].type; // Hauriem de tenir una taula amb tipus més endavant?

        // Vale ok podem ara llegir el nom de la variable i tal
        from++;

        // AQUI CAL FER UNA AMABLE LLISTA DE EXPRESSIONS I MODIFICAR-LES SEGONS EL TIPUS
        std::vector<Nodes::Node *> assignations;
        GetAssignations(from, to - 1, &assignations);

        // Modifiquem les assignacions per tal de fer declaracions
        for(int i = 0; i < assignations.size(); i++){
            
            assignations[i]->type = Nodes::NodeType::DECLARATION;
            assignations[i]->children.push_back(typeNode);
            
            // Posem les declaracions a dins!
            root->children.push_back(assignations[i]);
        }
    } else if(this->tokens[from].type == Token::IDENTIFIER && this->tokens[from + 1].type == Token::A_ASSIGMENT) {
        // Només assignations, res a veure
        std::vector<Nodes::Node *> assignations;
        GetAssignations(from, to-1, &assignations);
        for(int i = 0; i < assignations.size(); i++){
            root->children.push_back(assignations[i]);
        }
    } else {
        // És una expressió, la afegim simplement
        Nodes::Node* nextNode;
        GetExpression(from, to-1, &nextNode);
        root->children.push_back(nextNode);
    }
}

void Parser::GetAssignation(int from, int to, Nodes::Node** writeNode){
    // En principi from es l'identifier, despres va un igual, i després una expressió fins a to
    std::string id = this->tokens[from].value;
    from++;
    if(this->tokens[from].type != Token::TokenType::A_ASSIGMENT){
        // Throw exception
        Debug::LogError("Exception\n");
    }
    from++;
    Nodes::Node *exp;
    GetExpression(from, to, &exp);

    // Ara tenim a exp l'expressió, falta ara construir l'assignació final

    Nodes::Node *assignation = new Nodes::Node(Nodes::ASSIGNATION);
    assignation->SetDataString(id);
    assignation->children.push_back(exp);

    *writeNode = assignation;
}

void Parser::GetAssignations(int from, int to, std::vector<Nodes::Node *> *container){
    /*
    Tenim uns tokens de la forma
    a = 3, b = 5, c = "Hola que tal", d = f(2,3) + c, e = 4
    Volem separar les assignacions per comes i anar cridant GetAssignation
    */
    if(from > to) return;

    Nodes::Node *next;
    int tA = from;

    do {
        tA = GetNext(from, to+1, Token::TokenType::COMMA);
        // Val llavors tenim que:
        // a = 3 + 5, b = 
        // | f      | tA
        GetAssignation(from, tA-1, &next);
        container->push_back(next);

        // Ara desplaçem from
        from = tA+1;
    } while(from < to || tA < to);
}

bool Parser::Eat(Token::TokenType tok, Token::TokenType comp, int *from){
    if(*from >= this->tokens.size()) return false;

    if(tok == comp){
        *from += 1;
        return true;
    } else {
        // Call error, unexcepted identifier
        return false;
    }
}

int Parser::GetNext(int from, int lim, Token::TokenType type){
    // Trobem el proxim type respectant la jerarquia de parèntesis
    int j = 0;
    Token::TokenType t = this->tokens[from].type;
    while(from < this->tokens.size() && t != type){
        do {
            if(t == Token::TokenType::PARENTHESIS_CLOSE) j--;
            if(t == Token::TokenType::PARENTHESIS_OPEN) j++;

            if(t == Token::TokenType::CURLY_BRACKET_CLOSE) j--;
            if(t == Token::TokenType::CURLY_BRACKET_OPEN) j++;

            if(t == Token::TokenType::SQUARE_BRACKET_CLOSE) j--;
            if(t == Token::TokenType::SQUARE_BRACKET_OPEN) j++;
            
            from++;
            t = this->tokens[from].type;

            // Es podrien agafar excepcions si j < 0
        } while(j > 0);
    }
    if(from < lim || lim == -1) return from;
    else return lim;
}

void Parser::GetArguments(int from, int to, std::vector<Nodes::Node *>* parent){
    // Tenim expressions separades per comes, ex:
    // 2, 3, hola(), f() + 45
    // Volem passar-les a una llista guardada dins de parent

    // Cas aillat en cas que from > to
    if(from > to) return;

    Nodes::Node *next;
    int tA = from;

    do {
        tA = GetNext(from, to+1, Token::TokenType::COMMA);
        // Val llavors tenim que:
        // 3 + 5, f()
        // | f      | tA
        GetExpression(from, tA-1, &next);
        parent->push_back(next);

        // Ara desplaçem from
        from = tA+1;
    } while(from < to || tA < to);
}

void Parser::GetLiteral(int index, Nodes::Node** writeNode){
    Token token = this->tokens[index];
    Nodes::Node* lit = new Nodes::Node;
    switch(token.type){
        case Token::L_INT:
            lit->type = Nodes::NodeType::VAL_INT;
            lit->data.integer = std::stoi(token.value);
            *writeNode = lit;
            return;
        case Token::L_REAL:
            lit->type = Nodes::NodeType::VAL_REAL;
            lit->data.real = std::stod(token.value);
            *writeNode = lit;
            return;
        case Token::L_STRING:
            lit->type = Nodes::NodeType::VAL_STRING;
            lit->nd = token.value.size();
            lit->data.bytes = new char[lit->nd];
            for(int i = 0; i < lit->nd; i++){
                lit->data.bytes[i] = token.value[i];
            }
            *writeNode = lit;
            return;
        case Token::L_TRUE:
            lit->type = Nodes::NodeType::VAL_BOOLEAN;
            lit->data.integer = 1;
            *writeNode = lit;
            return;
        case Token::L_FALSE:
            lit->type = Nodes::NodeType::VAL_BOOLEAN;
            lit->data.integer = 0;
            *writeNode = lit;
            return;
        case Token::L_NULL:
            lit->type == Nodes::NodeType::VAL_NULL;
            *writeNode = lit;
            return;
        default:
            break;
    }
}

void Parser::GetExpression(int from, int to, Nodes::Node** writeNode){

    if(from == to){
        // En cas que la longitud sigui 1
        if(this->tokens[from].IsLiteral()){
            GetLiteral(from, writeNode);
        }

        if(this->tokens[from].IsIdentifier()){
            Nodes::Node *id = new Nodes::Node(Nodes::NodeType::REFERENCE);
            id->SetDataString(this->tokens[from].value);
            *writeNode = id;
        }

        return;
    }

    // Comprovem primer ara si això es una call
    if(this->tokens[from].IsIdentifier()){
        if(this->tokens[to].type == Token::PARENTHESIS_CLOSE){
            
            std::string value = this->tokens[from].value;
            
            from++;
            if (Eat(this->tokens[from].type, Token::PARENTHESIS_OPEN, &from)){
                Nodes::Node *call = new Nodes::Node(Nodes::NodeType::EXP_CALL);
                call->SetDataString(value);

                GetArguments(from, to - 1, &(call->children));
                *writeNode = call;

                return;
            }
        }
    }

    // Cas que els parèntesis siguin buits
    if(this->tokens[from].type == Token::PARENTHESIS_OPEN){
        if(this->tokens[to].type == Token::PARENTHESIS_CLOSE){
            int f = from+1;
            f = GetNext(f, -1, Token::PARENTHESIS_CLOSE);
            if(f == to){
                GetExpression(from + 1, to - 1, writeNode);
                return;
            }
        }
    }


    int i, k;

    for(i = opOrder.size() - 1; i >= 0; i--){
        for(k = 0; k < opOrder[i].size(); k++){
            int n = GetNext(from, to, opOrder[i][k]);
            // Val estem en la forma exp (simbol) exp, hem de trobar el simbol, estem al final doncs
            // iterem fins a trobar algun simbol potser
            if(n == from){
                // Ei és una operació unaria!
                Nodes::Node *unary = new Nodes::Node(Nodes::NodeType::EXP_UNARY);
                Nodes::Node *exp;

                unary->nd = (int) opOrder[i][k];

                GetExpression(from+1, to, &exp);
                unary->children.push_back(exp);
                *writeNode = unary;
                return;
            }
        }
    }
    
    
    for(i = opOrder.size() - 1; i >= 0; i--){
        for(k = 0; k < opOrder[i].size(); k++){
            int n = GetNext(from, to, opOrder[i][k]);
            if(n != to){ // L'hem trobat
                // from --- n simbol n --- to

                Nodes::Node *bin = new Nodes::Node(Nodes::NodeType::EXP_BINARY);
                Nodes::Node *first, *second;

                // Aqui suposo que s'haura de passar per algun map
                bin->nd = (int) opOrder[i][k];
                
                GetExpression(from, n - 1, &first);
                GetExpression(n + 1, to, &second);

                bin->children.push_back(first);
                bin->children.push_back(second);
                *writeNode = bin;

                return;
            }
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