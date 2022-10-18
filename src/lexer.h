#include <string>
#include <vector>
#include <map>

namespace GLexer {

    class Token {
        public:
            enum TokenType {
                ERROR,

                T_INT,                  // int a<1024> = 3
                T_RATIONAL,             // float q<32> = 2.3
                T_STRING,               // string s = "Hola"
                T_BOOLEAN,              // bool p = True
                T_NULL,                 // int b = NULL;

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
                PARENTHESIS,            //()
                SQUARE_BRACKET,         //[]
                CURLY_BRACKET,          //{}
                SEMICOLON,              //;

                K_IF,                   //if
                K_ELSE,                 //else
                K_WHILE,                //while
                K_RETURN,               //return
                K_BREAK,                //break
                K_CONTINUE,             //continue

                LITERAL,                // 3
                ARRAY_INDEXING,         // [3]
                FUNCTION_CALL,          // Amongus()
                FUNCTION_DECLARATION,   // Declarame esta loco
                FUNCTION,               // func

                IDENTIFIER

            };

            Token(TokenType type, std::string value);
            ~Token();

            std::string value;
            TokenType type;
    };

    class Lexer {
        enum Symbols {
            POINT,

        };

        std::map<Symbols, char> typeSymbol = {
            {POINT, '.'},
        };
       
        public:
            Lexer(std::string code);
            ~Lexer();
            std::vector<Token>* GetTokens();
            int GenerateTokens();
        private:
            int isNumber(char c);
            int GLexer::Lexer::isEmpty(char c);
            int GLexer::Lexer::isSemicolon(char c);
            int GLexer::Lexer::isSeparator(char c);

            Token getNumber(int pos, int *next);

            std::string code;
            std::vector<Token> tokenList;
    };

}