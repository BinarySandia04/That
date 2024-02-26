#include <iostream>
#include <cmath>

#ifndef _ZAGCXX
#include <string>
#include "ZagIR/Libs/Types.h"
#endif

namespace zagmath {
#ifdef _ZAGCXX
  double zsin(double r){
    return sin(r);
  }

  double zcos(double r){
    return cos(r);
  }
  
  double ztan(double r){
    return tan(r);
  }
#else
#endif
};
