#include "version.h"
#include "version-g.h"

#include <iostream>

int printVersion(){
    std::cout << "Radic interpreter version " << VERSION << " --- (c) Aran Roig "<< std::endl;
    return 0;
}