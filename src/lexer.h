#pragma once

#include <string>
#include <vector>
#include <map>

namespace That {

    class Token {
        public:
            enum TokenType {
                ERROR,

                T_INT,                  // int          X
                T_REAL,                 // real         X
                T_STRING,               // string       X
                T_BOOLEAN,              // bool         X

                // OP BINARIES
                S_PLUS,                 // +            X
                S_SUBTRACT,             // -            X
                S_MULTIPLY,             // *            X
                S_DIVIDE,               // /            X
                S_DIVIDES,              // |
                S_INTDIVIDE,            // //           X
                S_MODULO,               // %            X

                S_INCREMENT,            // ++           X
                S_DECREMENT,            // --           X
                S_FLIP,                 // !

                C_EQUAL,                // ==           X
                C_GREATER_THAN,         // >            X
                C_LESSER_THAN,          // <            X
                C_GREATER_EQUAL_THAN,   // >=           X
                C_LESSER_EQUAL_THAN,    // <=           X
                C_NOT_EQUAL,            // !=           X

                A_ASSIGMENT,            // =            X
                A_ADD,                  // +=           X
                A_SUBTRACT,             // -=           X
                A_MULTIPLY,             // *=           X
                A_DIVIDE,               // /=           X
                A_MODULO,               // %=           X

                COMMA,                  // ,        X
                POINT,                  // .        X
                PARENTHESIS_OPEN,       // (        X
                PARENTHESIS_CLOSE,      // )        X
                SQUARE_BRACKET_OPEN,    // [        X
                SQUARE_BRACKET_CLOSE,   // ]        X
                CURLY_BRACKET_OPEN,     // {        X
                CURLY_BRACKET_CLOSE,    // }        X
                DOLLAR,                 // $
                SEMICOLON,              // ;        X
                TWO_POINTS,             // :        X
                ARROW,                  // ->

                K_IF,                   //if        X
                K_ELSE,                 //else      X
                K_WHILE,                //while     X     
                K_RETURN,               //return    X
                K_BREAK,                //stop      X
                K_CONTINUE,             //skip      X

                L_INT,                  // 3        X
                L_REAL,                 // 3.2      X
                L_STRING,               // "Hola"   X
                L_TRUE,                 // True     X
                L_FALSE,                // False    X
                L_NULL,                 // Null     X

                FUNCTION_DECLARATION,   // func     X
                MODULE_DECLARATION,     // use      X
                IMPORT_DECLARATION,     // import   X

                IDENTIFIER              // algo     X

            };

            Token(TokenType type, std::string value);
            Token(TokenType type);

            ~Token();

            bool IsLiteral();

            std::string value;
            TokenType type;
    };

    class Lexer {
        enum Symbols {
            POINT,
            SPACE,
            TAB,
            NEWLINE,
            SEMICOLON,
            TWO_POINTS,
            COMMENT,
            DOUBLE_QUOT,
            QUOT,
            ESCAPE,
            COMMA,
            PARENTESIS_O,
            PARENTESIS_C,
            KEY_O,
            KEY_C,
            CLAUDATOR_O,
            CLAUDATOR_C,
            DOLLAR,
            BAR,
            
            MINUS,
            PLUS,
            MULTI,
            DIVIDE,
            PERCENT,
            FISH_O,
            FISH_C,
            EXCLAMATION,
        };

        enum Keywords {
            FUNC,
            MODULE,
            IMPORT,

            IF,
            ELSE,
            WHILE,
            RETURN,
            BREAK,
            CONTINUE,

            INT,
            REAL,
            STRING,
            BOOLEAN,
            TRUE,
            FALSE,
            _NULL,
        };

        std::map<std::string, Keywords> typeKeyword = {
            {"func", FUNC},
            {"use", MODULE},
            {"import", IMPORT},
            {"if", IF},
            {"else", ELSE},
            {"while", WHILE},
            {"return", RETURN},
            {"stop", BREAK},
            {"skip", CONTINUE},
            {"int", INT},
            {"real", REAL},
            {"string", STRING},
            {"bool", BOOLEAN},
            {"True", TRUE},
            {"False", FALSE},
            {"Null", _NULL}
        };

        std::map<Symbols, char> typeSymbol = {
            {POINT, '.'},
            {SPACE, ' '},
            {TAB, '\t'},
            {NEWLINE, '\n'},
            {SEMICOLON, ';'},
            {TWO_POINTS, ':'},
            {COMMENT, '#'},
            {DOUBLE_QUOT, '"'},
            {QUOT, '\''},
            {ESCAPE, '\\'},
            {COMMA, ','},
            {PARENTESIS_O, '('},
            {PARENTESIS_C, ')'},
            {KEY_O, '{'},
            {KEY_C, '}'},
            {CLAUDATOR_O, '['},
            {CLAUDATOR_C, ']'},
            {MINUS, '-'},
            {PLUS, '+'},
            {MULTI, '*'},
            {DIVIDE, '/'},
            {PERCENT, '%'},
            {FISH_O, '<'},
            {FISH_C, '>'},
            {EXCLAMATION, '!'},
            {DOLLAR, '$'},
            {BAR, '|'}
        };

        std::map<std::string, Token::TokenType> typeOperation = {
            {"+", Token::S_PLUS},
            {"-", Token::S_SUBTRACT},
            {"*", Token::S_MULTIPLY},
            {"/", Token::S_DIVIDE},
            {"//", Token::S_INTDIVIDE},
            {"%", Token::S_MODULO},
            {"++", Token::S_INCREMENT},
            {"--", Token::S_DECREMENT},
            {"=", Token::A_ASSIGMENT},
            {"==", Token::C_EQUAL},
            {"+=", Token::A_ADD},
            {"-=", Token::A_SUBTRACT},
            {"*=", Token::A_MULTIPLY},
            {"/=", Token::A_DIVIDE},
            {"%=", Token::A_MODULO},
            {">", Token::C_GREATER_THAN},
            {"<", Token::C_LESSER_THAN},
            {">=", Token::C_GREATER_EQUAL_THAN},
            {"<=", Token::C_LESSER_EQUAL_THAN},
            {"!=", Token::C_NOT_EQUAL},
            {"=>", Token::ARROW}
        };
       
        public:
            Lexer(std::string code);
            ~Lexer();
            std::vector<Token>* GetTokens();
            int GenerateTokens();
        private:
            std::string nextWord(int pos, int* nextPos);
            void flush(int* next);

            int isNumber(char c);
            int isEmpty(char c);
            int isSemicolon(char c);
            int isSeparator(char c);
            int isEnd(int i);
            int isComment(char c);
            int isPoint(char c);
            int isQuot(char c);
            int isDoubleQuot(char c);
            int isSymbol(char c);
            int isComma(char c);
            int isOParentesis(char c);
            int isCParentesis(char c);
            int isParentesis(char c);
            int isOClaudator(char c);
            int isCClaudator(char c);
            int isClaudator(char c);
            int isOKey(char c);
            int isCKey(char c);
            int isKey(char c);
            int isTwoPoints(char c);

            void skipComment(int *next);
            int checkLiterals(int *next);
            int checkKeywords(int *next);
            int checkSymbols(int *next);
            int checkOperations(int *next);

            int getNumber(int *next);
            void getString(int *next);

            void addError();

            std::string code;
            std::vector<Token> tokenList;
    };

}