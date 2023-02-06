#pragma once

#include <string>
#define DEBUG

namespace That {
    class Kernel {
        public:
            Kernel();
            ~Kernel();
            void compile(std::string code);
            void send(char filename[]);
            void sendScript(char name[]);
            void sendByteScript(char name[]);
    };
}
