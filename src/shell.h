#pragma once

#include <iostream>
#include "kernel.h"

namespace Glass {
    class Shell {
        public:
            Shell();
            ~Shell();
            static Shell CreateShell(Kernel *kernel);
    };
}
