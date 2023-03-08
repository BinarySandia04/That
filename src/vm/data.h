#pragma once

#include <cstdint>

#include <vector>

namespace That {

    typedef struct reg {  
        uint32_t num;
        uint8_t *data;

        enum type_t {
            INT,
            NUMBER,
            REAL,
            STRING,
            BOOLEAN,
            _NULL,
        } type;
        
    } reg_t;

    enum ParamType {
        E,
        A,
        AB,
        ABC,
    };

    enum InstructionID {
        LOAD,
        LOADC,

        PUSH, // abx
        CLOSE, // abx
        MOVE, // A, B
        MOVER, // A, B

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