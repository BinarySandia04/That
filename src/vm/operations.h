#pragma once

#include "data.h"

namespace That {
    namespace Operations {
        void IntIntSum(reg_t* a, reg_t* b, reg_t *c);
        void StrStrSum(reg_t* a, reg_t* b, reg_t *c);
        void IntStrSum(reg_t* a, reg_t* b, reg_t *c);
        void StrIntSum(reg_t* a, reg_t* b, reg_t *c);
        
        void IntIntMul(reg_t* a, reg_t* b, reg_t *c);
        void IntStrMul(reg_t* a, reg_t* b, reg_t *c);
        void StrIntMul(reg_t* a, reg_t* b, reg_t *c);

        void IntIntSub(reg_t* a, reg_t* b, reg_t *c);

        void IntIntDiv(reg_t* a, reg_t* b, reg_t *c);

        void IntIntMod(reg_t* a, reg_t* b, reg_t *c);

        void IntIntLt(reg_t* a, reg_t* b, reg_t *c);
        
        void IntIntLeq(reg_t* a, reg_t* b, reg_t *c);

        void IntIntGt(reg_t* a, reg_t* b, reg_t *c);

        void IntIntGeq(reg_t* a, reg_t* b, reg_t *c);

        void IntIntEq(reg_t* a, reg_t* b, reg_t *c);
    
        void IntIntNeq(reg_t* a, reg_t* b, reg_t *c);

        void BoolBoolAnd(reg_t* a, reg_t *b, reg_t *c);
    }
}