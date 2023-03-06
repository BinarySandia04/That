#pragma once

#include <vector>
#include <string>

#include "assembler.h"

namespace That {
    class Serializer {
        public:
            void Serialize(MachineCode assembly);
            void SerializeToFile(std::string fileName, MachineCode assembly);
        private:
            void WriteConst(FILE* f, reg_t reg);
            void WriteInstruction(FILE *f, Instruction ins);
    };
}