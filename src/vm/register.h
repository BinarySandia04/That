#pragma once

#include <cstdint>

namespace That {
    typedef struct reg {  
        uint32_t num;
        uint8_t *data;

        enum type_t {
            INT,
            NUMBER,
            REAL,
            STRING,
            BOOLEAN,
            _NULL,
        } type;
        
    } reg_t;

}