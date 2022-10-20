#include <string>
#include <vector>
#include <map>

namespace Glass {

    class Token {
        public:
            enum TokenType {
                ERROR,

                T_INT,                  // int
                T_REAL,                 // real
                T_STRING,               // string
                T_BOOLEAN,              // bool

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

                COMMA,                  //,
                PARENTHESIS_OPEN,       // (
                PARENTHESIS_CLOSE,      // )
                SQUARE_BRACKET_OPEN,    // [
                SQUARE_BRACKET_CLOSE,   // ]
                CURLY_BRACKET_OPEN,     // {
                CURLY_BRACKET_CLOSE,    // }
                SEMICOLON,              // ;

                K_IF,                   //if
                K_ELSE,                 //else
                K_WHILE,                //while
                K_RETURN,               //return
                K_BREAK,                //break
                K_CONTINUE,             //continue

                L_INT,                  // 3
                L_REAL,                 // 3.2
                L_STRING,               // "Hola"
                L_TRUE,                 // True
                L_FALSE,                // False
                L_NULL,                 // Null

                ARRAY_INDEXING,         // [3]
                FUNCTION_CALL,          // Amongus()
                FUNCTION_DECLARATION,   // func

                IDENTIFIER              // algo

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
            CLAUDATOR_C
        };

        enum Keywords {
            FUNC,
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
            {CLAUDATOR_C, ']'}
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
            void checkLiterals(int *next);
            void checkKeywords(int *next);
            void checkSymbols(int *next);

            void getNumber(int *next);
            void getString(int *next);

            void addError();

            std::string code;
            std::vector<Token> tokenList;
    };

}