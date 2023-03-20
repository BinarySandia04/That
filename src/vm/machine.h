#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <unordered_map>
#include <stack>

#include "internal.h"
#include "data.h"
#include "../flags/flags.h"

#define UINT32_SIZE 4

namespace That {
    class VM {
        
        public:
            VM(Flag::Flags flags);
            void Run(MachineCode code);
            
        private:
            // std::vector<reg_t> stack;
            Flag::Flags flags;

            // std::stack<int> offsets;
            std::vector<That::reg_t (*)(That::reg_t*, int)> defaultFunctions;
            std::vector<That::reg_t (*)(That::reg_t*, int)> internalFunctions;
            std::map<std::tuple<Type, Type>, That::reg_t (*)(That::reg_t*, That::reg_t*)> conversions;
            std::unordered_map<unsigned int, void (*)(reg_t*, reg_t*, reg_t*)> operations;

            void MemDump(uint8_t *data, int size);

            void Process(Instruction ins, int* current, std::vector<Constant> *constants, reg_t* registers);
            void Operate(OpType op, reg_t* a, reg_t* b, reg_t *c);

            std::string GetTypeName(Type t);
            std::string GetOperationName(OpType t);

            bool debug;

            
            void RegDump(reg_t* regs, int n);
    };
}