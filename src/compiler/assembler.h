#pragma once

#include "nodes.h"
#include "../kernel/kernel.h"
#include "../vm/machine.h"
#include "../vm/internal.h"

#include <map>
#include <vector>

/*
TODO: Val una manera de com construir aixo i ja tenir llenguatge estaria moolt bé
*/

namespace That {

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
    
    struct Identifier {
        std::string name;
    };

    class Assembler {
        public:
            void Assemble(Nodes::Node* ast, Flag::Flags flags);

            void AssembleFunction(Nodes::Node* func);
            void AssembleDeclaration(Nodes::Node* ident);
            void AssembleExpression(Nodes::Node* exp);

            void AppendReference(Nodes::Node* ref);

            void PushExpression(Nodes::Node* exp, std::vector<Instruction> set);
            
            void PushInstruction();
        private:
            std::vector<Identifier> identifiers;
            std::vector<Instruction> assembly;
    };
}