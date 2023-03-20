#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace That {

    enum Type {
        INT,
        NUMBER,
        REAL,
        STRING,
        BOOL,
        FUNCTION,
        _NULL,
    };

    enum OpType {
        OP_ADD,
        OP_SUBTRACT,
        OP_MUL,
        OP_DIV,
        OP_MOD,
        OP_LT,
        OP_GT,
        OP_LEQ,
        OP_GEQ,
        OP_NEQ,
        OP_EQ,
        OP_AND,
        OP_OR,
        OP_NOT
    };
    

    struct Reservation {
        std::string identifier;
        bool isFree;
        bool isIdentifier;

        Reservation(){
            isFree = false;
            isIdentifier = false;
        }
    };

    enum ParamType {
        E,
        A,
        AB,
        ABC,
    };

    enum InstructionID {
        LOADC,
        MOVE, // A, B

        CALL, // A, B, C // A(B+1, B+2, ..., B+C-1)
        DEF,
        ICL, // A, B, C // A(B+1,B+2,...,B+C-1) // Internal call
        RET, // Return something from reg A

        // Operacions
        // Important no canviar d'ordre
        ADD, // A = B + C
        SUB, // A = B - C
        MUL, // A = B * C
        DIV, // A = B / C
        MOD,

        AND,
        OR,
        NOT,

        EQ,
        NEQ,
        GT,
        LT,
        GEQ,
        LEQ,
        // ---------------------

        TO,
        END,
        JUMP,
        TEST,
        HALT,
    };

    class Instruction {
        public:
            Instruction();
            Instruction(InstructionID type, ParamType paramType);

            void SetA(int a);
            void SetB(int b);
            void SetC(int c);

            int GetA();
            int GetB();
            int GetC();

            InstructionID type;
            int temp;
            ParamType paramType;
        private:
            int a, b, c;
    };

    struct reg_t {
        int num;
        union {
            uint8_t *data;
            Instruction *instructions;
        };
        Type type;
    };

    struct Constant {
        reg_t data;  
    };

    struct MachineCode {
        std::vector<Constant> constants;
        std::vector<Instruction> instructions;
        int regCount;
    };
}