#pragma once

#include "register.h"

namespace That {
    namespace Internal {
        That::reg_t print(That::reg_t* a, That::reg_t* b);

        enum InternalFunctions {
            PRINT,
            READ,
        };
    }
}