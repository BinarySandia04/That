#pragma once

#include <cstdint>
#include <vector>

namespace That {
    class VM {
        typedef struct reg {
            union {
                uint32_t num;
                uint8_t *lnum;
            };
            enum type_t {
                OBJECT,
                INT,
                STR,
                NUMBER,
                NONE,
            } type;
        } reg_t;

        std::vector<reg_t*> regs;
        
        public:
            VM(char filename[]);
            ~VM();

            enum Instruction {
                PUSH, // A, Bx
                MOVE, // A, B
                POP // Bx
            };
            
        private:
            int Process(char ins[], reg_t* cons[]);
            uint8_t ReadA(char ins[]);
            uint8_t ReadB(char ins[]);
            uint8_t ReadC(char ins[]);
            uint32_t ReadBx(char ins[]);
            uint32_t ReadAbx(char ins[]);
    };
}