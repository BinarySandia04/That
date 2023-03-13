#pragma once

#include "nodes.h"
#include "../kernel/kernel.h"
#include "../vm/machine.h"
#include "../vm/data.h"

#include <map>
#include <vector>

/*
TODO: Val una manera de com construir aixo i ja tenir llenguatge estaria moolt bé
*/

namespace That {
    class Assembler {
        public:
            Assembler(Nodes::Node* ast);

            void AssembleCode(Nodes::Node* node, std::vector<Instruction> *to, std::vector<int> *declared);
            void AssembleFunction(Nodes::Node* func, std::vector<Instruction> *to);
            int AssembleDeclaration(Nodes::Node* ident, std::vector<Instruction> *to);
            int AssembleExpression(Nodes::Node* exp, std::vector<Instruction> *to);
            // int AssembleCall(Nodes::Node* call, std::vector<Instruction> *to);
            void AssembleDef(Nodes::Node* call, std::vector<Instruction> *to);
            void AssembleAssignation(Nodes::Node* assign, std::vector<Instruction> *to);
            void AssembleReturn(Nodes::Node* ret, std::vector<Instruction> *to);
            void AssembleConditional(Nodes::Node* cond, std::vector<Instruction> *to);
            void AssembleWhile(Nodes::Node* whil, std::vector<Instruction> *to);
            void AssembleFor(Nodes::Node* para, std::vector<Instruction> *to);
            void AssembleTempBreak(Nodes::Node *stop, std::vector<Instruction> *to);
            void AssembleTempSkip(Nodes::Node *skip, std::vector<Instruction> *to);

            // void AppendReference(Nodes::Node* ref);

            void PushInstruction(Instruction ins, std::vector<Instruction> *where);
            void PushInstructions(std::vector<Instruction> *from, std::vector<Instruction> *to);

            InstructionID TranslateBinOpId(int data);
            InstructionID TranslateUnOpId(int data);

            bool IsValue(Nodes::NodeType t);
            bool IsExpression(Nodes::NodeType t);

            // void IncreasePointer();
            // void DecreasePointer();
            void Free(int t);
            void FreeContext(std::vector<int> *v);

            int GetConstId(Nodes::Node *val);
            int GetRefId(std::string ref);
            int GetNextFree();

            MachineCode GetAssembly();
        private:
            // std::vector<std::string> identifierStack;
            
            std::vector<Instruction> instructions;
            std::vector<Constant> constants;

            std::vector<Reservation> reserves;

            // int regCount = 1;
            // int regPointer = 0;
            // std::stack<int> stacks;
    };
}