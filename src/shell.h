#pragma once

#include <iostream>
#include "kernel.h"

namespace Radic {
    class Shell {
        public:
            Shell();
            ~Shell();
            static void CreateShell(Kernel *kernel);
    };
}
