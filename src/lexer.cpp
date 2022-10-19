#include <string>
#include "lexer.h"

Glass::Token::Token(TokenType type, std::string value){
    this->type = type;
    this->value = value;
}

Glass::Token::~Token(){

}

Glass::Lexer::Lexer(std::string code){
    this->code = code;
}

Glass::Lexer::~Lexer(){
}

/*
De un codi doncs torna tokens!
*/
std::vector<Glass::Token>* Glass::Lexer::GetTokens(){
    return &tokenList;
}


int Glass::Lexer::isNumber(char c){
    if(c >= 48 && c <= 57) return true;
    return false;
}

int Glass::Lexer::isEmpty(char c){
    return c == typeSymbol[Symbols::SPACE] || c == typeSymbol[Symbols::NEWLINE];
}

int Glass::Lexer::isSemicolon(char c){
    return c == typeSymbol[Symbols::SEMICOLON];
}

int Glass::Lexer::isSeparator(char c){
    return 0;
}

void Glass::Lexer::skipComment(int pos, int *next){
    while(code[pos] != typeSymbol[Symbols::NEWLINE] && !isEnd(pos)){
        pos++;
    }
    *next = pos + 1;
}

int Glass::Lexer::isComment(char c){
    return c == typeSymbol[Symbols::COMMENT];
}

int Glass::Lexer::isEnd(int pos){
    return pos >= code.size();
}

Glass::Token Glass::Lexer::getNumber(int pos, int *next){
    std::string num = "" + code[pos];
    int isRational = 0;

    for(; isNumber(code[pos]); pos++){
        num += code[pos];
    }

    if(code[pos] == typeSymbol[Symbols::POINT]){
        num += ".";
        isRational = 1;
        for(pos += 1; isNumber(code[pos]); pos++){
            num += code[pos];
        }
    }

    *next = pos;
    if(isEmpty(code[pos]) || isSemicolon(code[pos]) || isSeparator(code[pos]) || isEnd(pos)){
        if(isRational) return Token(Token::T_RATIONAL, num);
        else return Token(Token::T_INT, num);
    }
    return Token(Token::ERROR, "");
    // Error
}

int Glass::Lexer::GenerateTokens(){
    for(int i = 0; i < code.size(); i++){
        char c = code[i];

        if(isComment(c)){
            skipComment(i, &i);
        }

        if(isNumber(c)){
            tokenList.push_back(getNumber(i, &i));
        }
    }

    return 0;
}