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
        _NULL,
    };

    enum Operator {
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_MOD,

        OP_NOT,
        OP_EQ,
        OP_NEQ,
        OP_GT,
        OP_LT,
        OP_GEQ,
        OP_LEQ,
        OP_AND,
        OP_OR,
    };

    struct reg_t {  
        int num;
        uint8_t *data;
        Type type;
    };

    struct Reservation {
        std::string identifier;
        bool isFree;
        bool isIdentifier;
        int hasConstant;

        Reservation(){
            isFree = false;
            isIdentifier = false;
            hasConstant = -1;
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

    struct Constant {
        reg_t data;  
    };

    struct MachineCode {
        std::vector<Constant> constants;
        std::vector<Instruction> instructions;
        int regCount;
    };
}