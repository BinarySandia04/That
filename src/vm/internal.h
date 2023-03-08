#pragma once

#include "data.h"

namespace That {
    namespace Internal {
        That::reg_t print(That::reg_t* a, int size);

        enum InternalFunctions {
            PRINT,
            READ,
        };
    }
}