#include <string>
#include <vector>
#include <map>

namespace Glass {

    class Token {
        public:
            enum TokenType {
                ERROR,

                T_INT,                  // int          X
                T_REAL,                 // real         X
                T_STRING,               // string       X
                T_BOOLEAN,              // bool         X

                S_PLUS,                 // +
                S_MINUS,                // -
                S_MULTIPLY,             // *
                S_DIVIDE,               // /
                S_INTDIVIDE,            // //
                S_MODULO,               // %
                S_INCREMENT,            // ++
                S_DECREMENT,            // --

                C_EQUAL,                // ==
                C_GREATER_THAN,         // >
                C_LESSER_THAN,          // <
                C_GREATER_EQUAL_THAN,   // >=
                C_LESSER_EQUAL_THAN,    // <=
                C_NOT_EQUAL,            // !=

                A_ASSIGMENT,            // =
                A_ADD,                  // +=
                A_SUBTRACT,             // -=
                A_MULTIPLY,             // *=
                A_DIVIDE,               // /=
                A_MODULO,               // %=

                COMMA,                  // ,        X
                PARENTHESIS_OPEN,       // (        X
                PARENTHESIS_CLOSE,      // )        X
                SQUARE_BRACKET_OPEN,    // [        X
                SQUARE_BRACKET_CLOSE,   // ]        X
                CURLY_BRACKET_OPEN,     // {        X
                CURLY_BRACKET_CLOSE,    // }        X
                SEMICOLON,              // ;        X

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

                IDENTIFIER              // algo     X

            };

            Token(TokenType type, std::string value);
            Token(TokenType type);
            ~Token();

            std::string value;
            TokenType type;
    };

    class Lexer {
        enum Symbols {
            POINT,
            SPACE,
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
            MINUS,
        };

        enum Keywords {
            FUNC,
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

        enum Operations {
            ADD,
            SUBTRACT,
            MULTIPLY,
            DIVIDE,
        };

        std::map<std::string, Keywords> typeKeyword = {
            {"func", FUNC},
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
            {MINUS, '-'}
        };

        std::map<std::string, Operations> typeOperation = {
            {"+", ADD},
            {"-", SUBTRACT},
            {"*", MULTIPLY},
            {"/", DIVIDE},
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
            int isSign(char c);

            void skipComment(int *next);
            void checkLiterals(int *next);
            void checkKeywords(int *next);
            void checkSymbols(int *next);
            void checkOperations(int *next);

            void getNumber(int *next);
            void getString(int *next);

            void addError();

            std::string code;
            std::vector<Token> tokenList;
    };

}