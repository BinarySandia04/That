#pragma once

#include <iostream>
#include "kernel.h"

class GShell {
    public:
        GShell();
        ~GShell();
        static GShell CreateShell(GKernel *kernel);
};