#include "version.h"
#include "version-g.h"
#include "headers/termcolor.hpp"

#include <iostream>

int printVersion(){
    std::cout << termcolor::red << termcolor::bold << "That" << termcolor::reset << " interpreter version " << VERSION << " --- (c) Aran Roig "<<
     termcolor::reset << std::endl;
    return 0;
}