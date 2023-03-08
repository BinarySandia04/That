#pragma once

#include <vector>
#include <string>

#include "assembler.h"

namespace That {
    class Serializer {
        public:
            void Serialize(MachineCode assembly);
            void SerializeToFile(std::string fileName, MachineCode assembly);
            void SerializeFromFile(std::string fileName, MachineCode *code);
        private:
            void WriteConst(FILE* f, reg_t reg);
            void WriteInstruction(FILE *f, Instruction ins);
            void ReadConst(FILE *f, std::vector<Constant> *constants);
            bool ReadInstruction(FILE *f, std::vector<Instruction> *instructions);
    };
}