#pragma once

#include <cstdint>
#include <stack>

namespace Radic {
    class VM {
        typedef struct reg {
            union {
                uint32_t num;
                char *str;
                uint32_t *lnum;
            };
            enum {
                INT,
                STR,
                LONG
            } type;
        } reg_t;

        std::stack<reg_t> regs;
        
        public:
            VM(char filename[]);
            ~VM();

            enum Instruction {
                LOADC, // A, Bx
                MOVE
            };
            
        private:
            int Process(char ins[], uint64_t cons[]);
            uint8_t ReadA(char ins[]);
            uint8_t ReadB(char ins[]);
            uint8_t ReadC(char ins[]);
            unsigned int ReadBx(char ins[]);
    };
}