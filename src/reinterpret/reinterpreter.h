#pragma once

#include <string>
#include "../compiler/nodes.h"

namespace That {
    class Reinterpreter {
        public:
            Reinterpreter(Nodes::Node *ast);
            std::string GetCode();
        private:
            Nodes::Node *ast;
    };
}
