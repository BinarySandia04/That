#pragma once

#include "data.h"

namespace That {
    namespace Operations {
        reg_t IntIntSum(reg_t* a, reg_t* b);
        reg_t StrStrSum(reg_t* a, reg_t* b);
        reg_t IntStrSum(reg_t* a, reg_t* b);
        reg_t StrIntSum(reg_t* a, reg_t* b);
        
        reg_t IntIntMul(reg_t* a, reg_t* b);
        reg_t IntStrMul(reg_t* a, reg_t* b);
        reg_t StrIntMul(reg_t* a, reg_t* b);

        reg_t IntIntSub(reg_t* a, reg_t* b);

        reg_t IntIntDiv(reg_t* a, reg_t* b);

        reg_t IntIntMod(reg_t* a, reg_t* b);

        reg_t IntIntLt(reg_t* a, reg_t* b);
        
        reg_t IntIntLeq(reg_t* a, reg_t* b);

        reg_t IntIntGt(reg_t* a, reg_t* b);

        reg_t IntIntGeq(reg_t* a, reg_t* b);

        reg_t IntIntEq(reg_t* a, reg_t* b);
    
        reg_t IntIntNeq(reg_t* a, reg_t* b);

        reg_t BoolBoolAnd(reg_t* a, reg_t *b);
    }
}