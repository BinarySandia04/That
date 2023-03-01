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

    class Instruction {
        public:
            Instruction();
            VM::Instructions type;
            uint8_t ins[4];

            int a, b, x;
    };

    struct ConstantList {
        int elements;
    };

    struct Function {
        ConstantList elements;
    };

    class Assembler {
        public:
            void Assemble(Nodes::Node* ast, Flag::Flags flags);

            void AssembleFunction(Nodes::Node* func);
            void AssembleDeclaration(Nodes::Node* ident);
            void AssembleExpression(Nodes::Node* exp);
            void AssembleCall(Nodes::Node* call);

            void AppendReference(Nodes::Node* ref);

            void PushInstruction(Instruction ins);

            VM::Instructions TranslateBinOpId(int data);
            VM::Instructions TranslateUnOpId(int data);

            bool IsValue(Nodes::Node* n);

            void IncreasePointer();
            void DecreasePointer();

            int GetConstId(Nodes::Node *val);
            int GetRefId(std::string ref);
        private:
            std::vector<std::string> identifierStack;
            std::vector<Instruction> assembly;

            int regCount = 1;
            int regPointer = 0;
    };
}