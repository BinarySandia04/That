#pragma once

#include <string>

namespace Glass {
    class Kernel {
        public:
            Kernel();
            ~Kernel();
            void send(std::string code);
    };
}
