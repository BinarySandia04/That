#pragma once

#include <string>
#define DEBUG

namespace Rux {
    class Kernel {
        public:
            Kernel();
            ~Kernel();
            void send(std::string code);
            void sendScript(char name[]);
    };
}
