#pragma once

#include "nodes.h"
#include "../vm/machine.h"

#include <map>

/*
TODO: Val una manera de com construir aixo i ja tenir llenguatge estaria moolt bé
*/

namespace That {
    class Assembler {
        public:
            void Assemble(That::Nodes::Node* ast);

            void AssembleFunction(That::Nodes::Node* func);

            void AppendReference(That::Nodes::Node* ref);
        private:
            std::map<std::string, int> identifiers;
    };

    struct Instruction {
        VM::Instructions inst;
        uint8_t ins[4];
    };

    struct ConstantList {
        int elements;
    };

    struct Function {
        ConstantList elements;

    };
}