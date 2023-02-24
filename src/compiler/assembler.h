#pragma once

#include "nodes.h"
#include "../vm/machine.h"

/*
TODO: Val una manera de com construir aixo i ja tenir llenguatge estaria moolt bé
*/

namespace That {
    class Assembler {
        public:
            Assembler();

            void Assemble(That::Nodes::Node* ast);
    };

    class Instruction {

    };

    class ConstantList {
        int elements;
    };
}