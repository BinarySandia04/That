#pragma once

#include <vector>
#include <string>

#include "lexer.h"

namespace That {
    namespace Nodes {
        enum NodeType {
            NODE,
            ERROR,
            DECLARATION,
            ASSIGNATION,
            REFERENCE,
            PARAMETER,
            TYPE,
            IF,
            WHILE,
            FOR,
            BREAK,
            SKIP,
            RETURN,
            FUNCTION,
            DEF_FUNCTION,
            NODE_ERROR,
            EXP_BINARY,
            EXP_UNARY,
            EXP_CALL,
            VAL_INT,
            VAL_BIGINT,
            VAL_REAL,
            VAL_STRING,
            VAL_BOOLEAN,
            VAL_NULL
        };

        enum OpType {
            ADD,
            SUBTRACT,
            MULT,
            DIV,
            MOD
        };

        class Node {

            public:
                // Node(NodeType type);
                // void Execute(); // Això per execució un cop construida la estructura del codi
                Node(NodeType);
                Node();
                ~Node();

                std::vector<Node*> children;
                NodeType type;

                bool allocable = false;
                int nd;
                union data {
                    int integer;
                    double real;
                    char* bytes;
                } data;

                void Debug();

                bool IsValue();
                bool IsExpression();

                void SetDataString(std::string s);
                std::string GetDataString();
        };
    }
}
 