#pragma once

#include "data.h"

#include <vector>
#include <map>
#include <tuple>

namespace That {
    namespace Internal {
        That::reg_t print(That::reg_t* a, int size);

        enum InternalFunctions {
            PRINT,
            READ,
        };

        void LoadDefaultFunctions(std::vector<That::reg_t (*)(That::reg_t*, int)> *func);
        void LoadInternalFunctions(std::vector<That::reg_t (*)(That::reg_t*, int)> *func);
        void LoadConversions(std::map<std::tuple<Type, Type>, That::reg_t (*)(That::reg_t*, That::reg_t*)> *conv);
        void LoadOperations(std::map<std::tuple<Operator, Type>, reg_t (*)(reg_t*, reg_t*)> *ops);
    }
}