#pragma once

#include <iostream>
#include "kernel.h"

namespace Rux {
    class Shell {
        public:
            Shell();
            ~Shell();
            static void CreateShell(Kernel *kernel);
    };
}
