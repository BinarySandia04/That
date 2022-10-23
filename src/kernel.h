#pragma once

#include <string>
#define DEBUG

namespace Glass {
    class Kernel {
        public:
            Kernel();
            ~Kernel();
            void send(std::string code);
            void sendScript(char name[]);
    };
}
