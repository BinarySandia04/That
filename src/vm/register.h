#pragma once

#include <cstdint>

namespace That {
    typedef struct reg {  
        uint32_t num;
        uint8_t *data;

        enum type_t {
            OBJECT,
            INT,
            STR,
            NUMBER,
            FUNCTION,
            NONE,
        } type;
    } reg_t;
}