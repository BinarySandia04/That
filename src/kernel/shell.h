#pragma once

#include <iostream>
#include "kernel.h"

namespace That {
    class Shell {
        public:
            Shell();
            ~Shell();
            static void CreateShell(Kernel *kernel);
    };
}
