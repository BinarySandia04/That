#pragma once

#include <vector>

#include "lexer.h"
#include "nodes.h"
#include "../vm/data.h"

namespace That {

    class Parser {
        public:
            Parser(std::vector<That::Token> tokens);

            Nodes::Node* GetAST();
        private:
            std::vector<That::Token> tokens;

            std::vector<std::vector<That::Token::TokenType>> opOrder = {
                {Token::S_MODULO},
                {Token::S_MULTIPLY, Token::S_DIVIDE},
                {Token::S_PLUS, Token::S_SUBTRACT},
                {Token::C_GREATER_THAN, Token::C_LESSER_THAN, Token::S_NOT},
                {Token::C_NOT_EQUAL, Token::C_EQUAL, Token::C_GREATER_EQUAL_THAN, Token::C_LESSER_EQUAL_THAN,
                Token::S_AND, Token::S_OR}
            };

            std::vector<That::Token::TokenType> types = {
                Token::T_INT, Token::T_BOOLEAN, Token::T_REAL, Token::T_STRING
            };

            std::vector<That::Token::TokenType> assignations = {
                Token::A_ASSIGMENT,
                Token::A_ADD,
                Token::A_SUBTRACT,
                Token::A_MULTIPLY,
                Token::A_DIVIDE,
                Token::A_MODULO
            };
            
            std::map<That::Token::TokenType, That::Token::TokenType> opMap = {
                {Token::A_ADD, Token::S_PLUS},
                {Token::A_SUBTRACT, Token::S_SUBTRACT},
                {Token::A_DIVIDE, Token::S_DIVIDE},
                {Token::A_MULTIPLY, Token::S_MULTIPLY},
                {Token::A_MODULO, Token::S_MODULO}                
            };

            Nodes::Node *root;

            void GenerateCode(int from, int to, Nodes::Node *parent);

            void ThrowError();

            void GetCodeFunction(Nodes::Node **root, int from, int *end);
            void GetCodeLine(Nodes::Node *root, int from, int to);
            void GetCodeConditional(Nodes::Node **root, int from, int *end);
            void GetCodeWhile(Nodes::Node **root, int from, int *end);
            void GetCodeReturn(Nodes::Node **root, int from, int *end);
            void GetCodeFor(Nodes::Node **root, int from, int *end);
            void GetCodeBreak(Nodes::Node **root, int from, int *end);
            void GetCodeSkip(Nodes::Node **root, int from, int *end);

            bool Eat(int pos, Token::TokenType comp, int *from);

            int GetNext(int from, int lim, Token::TokenType type);
            int GetNextCodeSep(int from, int lim);

            void GetArguments(int from, int to, std::vector<Nodes::Node *>* parent);
            void GetFunctionParameters(int from, int to, std::vector<Nodes::Node *>* container);
            void GetFunctionParameter(int from, int to, Nodes::Node** writeNode);
            void GetAssignation(int from, int to, Nodes::Node** writeNode);
            void GetAssignations(int from, int to, std::vector<Nodes::Node *> *container);
            void GetConditional(int from, int* to, Nodes::Node* pushNode);
            void GetCodeBlock(int from, int* to, Nodes::Node* parent);
            
            void GetExpression(int from, int to, Nodes::Node** writeNode);
            void GetLiteral(int index, Nodes::Node** writeNode);

            bool CodeLoop(int *from, int *nF, Nodes::Node *parent);

            bool ContainsAssignation(int from, int to);

            bool IsOf( std::vector<Token::TokenType> list, Token::TokenType type);
            
            Type GetTypeFromToken(int t);
            OpType GetOpFromToken(Token::TokenType t);
    };
}