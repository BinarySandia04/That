#include <iostream>
#include <fstream>
#include <limits>
#include <cstdint>

#include <gmp.h>

#include "kernel.h"
#include "shell.h"
#include "../headers/termcolor.hpp"
#include "../compiler/lexer.h"
#include "../compiler/parser.h"
#include "../compiler/serializer.h"
#include "../compiler/assembler.h"
#include "../vm/machine.h"
#include "../version.h"
#include "../flags/flags.h"

using namespace That;

Kernel::Kernel() {
    /* Constructor */
}

Kernel::~Kernel() {
    /* Destructor */
}

void Kernel::DebugTokens(std::vector<Token> tokens){
    std::cout << termcolor::red << termcolor::bold << "Tokens:" << termcolor::reset << std::endl;
    std::map<That::Token::TokenType, std::string> mapo = {
        {Token::TokenType::ERROR, "ERROR"},
        {Token::TokenType::T_INT, "T_INT"},                  // int          X
        {Token::TokenType::T_REAL, "T_REAL"},                 // real         X
        {Token::TokenType::T_STRING, "T_STRING"},               // string       X
        {Token::TokenType::T_BOOLEAN, "T_BOOLEAN"},              // bool         X

        {Token::TokenType::S_PLUS, "S_PLUS"},                 // +            X
        {Token::TokenType::S_SUBTRACT, "S_SUBTRACT"},             // -            X
        {Token::TokenType::S_MULTIPLY, "S_MULTIPLY"},             // *            X
        {Token::TokenType::S_DIVIDE, "S_DIVIDE"},               // /            X
        {Token::TokenType::S_MODULO, "S_MODULO"},               // %            X

        {Token::TokenType::S_INCREMENT, "S_INCREMENT"},            // ++           X
        {Token::TokenType::S_DECREMENT, "S_DECREMENT"},            // --           X
        {Token::TokenType::S_NOT, "S_NOT"},                 // !

        {Token::TokenType::S_AND, "S_AND"},
        {Token::TokenType::S_OR, "S_OR"},

        {Token::TokenType::C_EQUAL, "C_EQUAL"},                // ==           X
        {Token::TokenType::C_GREATER_THAN, "C_GREATER_THAN"},         // >            X
        {Token::TokenType::C_LESSER_THAN, "C_LESSER_THAN"},          // <            X
        {Token::TokenType::C_GREATER_EQUAL_THAN, "C_GREATER_EQUAL_THAN"},   // >=           X
        {Token::TokenType::C_LESSER_EQUAL_THAN, "C_LESSER_EQUAL_THAN"},    // <=           X
        {Token::TokenType::C_NOT_EQUAL, "C_NOT_EQUAL"},            // !=           X

        {Token::TokenType::A_ASSIGMENT, "A_ASSIGMENT"},            // =            X
        {Token::TokenType::A_ADD, "A_ADD"},                  // +=           X
        {Token::TokenType::A_SUBTRACT, "A_SUBTRACT"},             // -=           X
        {Token::TokenType::A_MULTIPLY, "A_MULTIPLY"},             // *=           X
        {Token::TokenType::A_DIVIDE, "A_DIVIDE"},               // /=           X
        {Token::TokenType::A_MODULO, "A_MODULO"},               // %=           X

        {Token::TokenType::COMMA, "COMMA"},                  // ,        X
        {Token::TokenType::POINT, "POINT"},                  // .        X
        {Token::TokenType::PARENTHESIS_OPEN, "PARENTHESIS_OPEN"},       // (        X
        {Token::TokenType::PARENTHESIS_CLOSE, "PARENTHESIS_CLOSE"},      // )        X
        {Token::TokenType::SQUARE_BRACKET_OPEN, "SQUARE_BRACKET_OPEN"},    // [        X
        {Token::TokenType::SQUARE_BRACKET_CLOSE, "SQUARE_BRACKET_CLOSE"},   // ]        X
        {Token::TokenType::CURLY_BRACKET_OPEN, "CURLY_OPEN"},     // {        X
        {Token::TokenType::CURLY_BRACKET_CLOSE, "CURLY_CLOSE"},    // }        X
        {Token::TokenType::DOLLAR, "DOLLAR"},                 // $
        {Token::TokenType::SEMICOLON, "SEMICOLON"},              // ;        X
        {Token::TokenType::TWO_POINTS, "TWO_POINTS"},             // :        X
        {Token::TokenType::ARROW, "ARROW"},                  // ->

        {Token::TokenType::K_IF, "IF"},                   //if        X
        {Token::TokenType::K_ELSE, "ELSE"},                 //else      X
        {Token::TokenType::K_WHILE, "WHILE"},                //while     X     
        {Token::TokenType::K_RETURN, "RETURN"},               //return    X
        {Token::TokenType::K_BREAK, "BREAK"},                //stop      X
        {Token::TokenType::K_CONTINUE, "CONTINUE"},             //skip      X
        {Token::TokenType::K_FOR, "FOR"},

        {Token::TokenType::L_INT, "L_INT"},                  // 3        X
        {Token::TokenType::L_REAL, "L_REAL"},                 // 3.2      X
        {Token::TokenType::L_STRING, "L_STRING"},               // "Hola"   X
        {Token::TokenType::L_TRUE, "L_TRUE"},                 // True     X
        {Token::TokenType::L_FALSE, "L_FALSE"},                // False    X
        {Token::TokenType::L_NULL, "L_NULL"},                 // Null     X

        {Token::TokenType::FUNCTION_DECLARATION, "FUNC"},   // func     X
        {Token::TokenType::MODULE_DECLARATION, "USE"},     // use      X
        {Token::TokenType::IMPORT_DECLARATION, "IMPORT"},     // import   X

        {Token::TokenType::IDENTIFIER, "ID"}              //  algo     X

    };

    for(int i = 0; i < tokens.size(); i++){
        std::cout << "[";
        std::cout << "type: " << mapo[tokens[i].type];
        if(tokens[i].value.size() > 0){
            std::cout << ", value: " << tokens[i].value << "]";
        } else {
            std::cout << "]";
        }
        if(i < tokens.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}

void Kernel::RunScript(std::string name, Flag::Flags flags){
    std::fstream file(name);

    std::string code = "", line;
    while(std::getline(file, line)){
        code += line + "\n";
    }

    That::Lexer lexer(code);

    lexer.GenerateTokens();
    std::vector<Token> tokens = *(lexer.GetTokens());

    if(CHECK_BIT(flags, 1)){
        DebugTokens(tokens);
    }

    Parser parser(tokens);

    Nodes::Node *ast = parser.GetAST();

    if(CHECK_BIT(flags, 1)){
        std::cout << termcolor::red << termcolor::bold << "AST:" << termcolor::reset << std::endl;
        ast->Debug();
        std::cout << std::endl;
    }

    Assembler assembler(ast);
    MachineCode machineCode = assembler.GetAssembly();
    
    if(CHECK_BIT(flags, 1)){
        std::cout << termcolor::red << termcolor::bold << "ASM:" << termcolor::reset << std::endl;
        // Ara doncs fem debug de les instruccions
        for(int i = 0; i < machineCode.instructions.size(); i++){
            std::cout << machineCode.instructions[i].type << " ";
            if(machineCode.instructions[i].GetA() != INT32_MIN) std::cout << machineCode.instructions[i].GetA() << " ";
            if(machineCode.instructions[i].GetB() != INT32_MIN) std::cout << machineCode.instructions[i].GetB() << " ";
            if(machineCode.instructions[i].GetC() != INT32_MIN) std::cout << machineCode.instructions[i].GetC() << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    
    /*
    Serializer serializer;
    serializer.SerializeToFile("a.th", machineCode);
    serializer.SerializeFromFile("a.th", &machineCode);
    */

    VM machine;
    // TODO: Hacer esto
    if(CHECK_BIT(flags, 1)){
        std::cout << termcolor::red << termcolor::bold << "EXEC:" << termcolor::reset << std::endl;
    }
    machine.Run(machineCode);


    delete ast;
}


void Kernel::Cli(){
    Shell::CreateShell(this);
    return;
}

void Kernel::PrintVersion(){
    printVersion();
}