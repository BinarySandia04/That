#pragma once

#include <iostream>
#include "kernel.h"

namespace Glass {
    class Shell {
        public:
            Shell();
            ~Shell();
            static void CreateShell(Kernel *kernel);
    };
}
