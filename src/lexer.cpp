#include <string>
#include "lexer.h"

GLexer::Token::Token(TokenType type, std::string value){
    this->type = type;
    this->value = value;
}

GLexer::Token::~Token(){

}

GLexer::Lexer::Lexer(std::string code){
    this->code = code;
}

GLexer::Lexer::~Lexer(){
}

/*
De un codi doncs torna tokens!
*/
std::vector<GLexer::Token>* GLexer::Lexer::GetTokens(){
    return &tokenList;
}


int GLexer::Lexer::isNumber(char c){
    if(c >= 48 && c <= 57) return true;
    return false;
}

int GLexer::Lexer::isEmpty(char c){

}

int GLexer::Lexer::isSemicolon(char c){
    
}

int GLexer::Lexer::isSeparator(char c){
    
}

GLexer::Token GLexer::Lexer::getNumber(int pos, int *next){
    std::string num = "" + code[pos];
    int isRational = 0;

    for(pos += 1; isNumber(code[pos]); pos++){
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
    if(isEmpty(code[pos]) || isSemicolon(code[pos]) || isSeparator(code[pos])){
        if(isRational) return Token(Token::T_RATIONAL, num);
        else return Token(Token::T_INT, num);
    }
    return Token(Token::ERROR, "");
    // Error
}

int GLexer::Lexer::GenerateTokens(){
    for(int i = 0; i < code.size(); i++){
        char c = code[i];

        if(isNumber(c)){
            tokenList.push_back(getNumber(i, &i));
        }
    }
}