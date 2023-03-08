#pragma once

#include <cstdint>
#include <vector>
#include <map>

#include "internal.h"
#include "data.h"

#define UINT32_SIZE 4

namespace That {
    class VM {
        
        public:
            void Run(MachineCode code);
            static std::map<That::Internal::InternalFunctions, That::reg_t (*)(That::reg_t*, int)> FunctionMap;
        private:
            
            reg_t* registers;
            MachineCode currentCode;

            void MemDump(uint8_t *data, int size);

            void Process(Instruction ins);
    };
}