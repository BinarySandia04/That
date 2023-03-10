#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <stack>

#include "internal.h"
#include "data.h"

#define UINT32_SIZE 4

namespace That {
    class VM {
        
        public:
            void Run(MachineCode code);
        private:
            reg_t* registers;
            std::vector<reg_t> stack;
            int stackOffset;

            std::stack<int> offsets;
            std::vector<That::reg_t (*)(That::reg_t*, int)> defaultFunctions;
            std::vector<That::reg_t (*)(That::reg_t*, int)> internalFunctions;
            std::map<std::tuple<Type, Type>, That::reg_t (*)(That::reg_t*, That::reg_t*)> conversions;
            std::map<std::tuple<Operator, Type, Type>, reg_t (*)(reg_t*, reg_t*)> operations;

            MachineCode currentCode;

            void MemDump(uint8_t *data, int size);

            void Process(Instruction ins, int* current);
            reg_t Operate(Operator op, reg_t* a, reg_t* b);

            std::string GetTypeName(Type t);
            std::string GetOperationName(Operator t);

            
            void RegDump();
            void StackDump();
    };
}