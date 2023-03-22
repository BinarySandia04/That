#pragma once

#include <string>
#include "../compiler/nodes.h"

namespace That {
    class Reinterpreter {
        public:
            std::string GetCode(Nodes::Node *ast);
        private:
            Nodes::Node *ast;

            std::string GetCodeBlock(Nodes::Node *ast);

            std::string AssembleExpression(Nodes::Node *n);
            std::string AssembleDef(Nodes::Node *n);
            std::string AssembleDeclaration(Nodes::Node *n);
            std::string AssembleAssignation(Nodes::Node *n);
            // else if(t == Nodes::RETURN) AssembleReturn(n, to);
            std::string AssembleConditional(Nodes::Node *n);
            std::string AssembleWhile(Nodes::Node *n);
            std::string AssembleFor(Nodes::Node *n);
            std::string AssembleTempBreak(Nodes::Node *n);
            std::string AssembleTempSkip(Nodes::Node *n);


            std::string GetOperationGraphic(int n);
            std::string GetLiteralValue(Nodes::Node *val);
            std::string GetType(Nodes::Node *t);
    };
}
