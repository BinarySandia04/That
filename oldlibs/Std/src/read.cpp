// Libs que fa servir el copypaste
#include <iostream>
#include <string>

// _ZAGCXX definit significa que aixo fa copy paste al transpiler
// si no està definit el que fa es fer extern "C" i va al IR per altres
// coses intermitges
#ifndef _ZAGCXX
#include <string>
#include "ZagIR/Libs/Types.h"
#endif

namespace zagstd {
#ifdef _ZAGCXX
  std::string read_str(std::string data){
    std::string readVar;
    std::cout << data;
    std::cin >> readVar;
    return readVar;
  }

  int read_int(std::string data){
    int readVar;
    std::cout << data;
    std::cin >> readVar;
    return readVar;
  }

  double read_num(std::string data){
    double readVar;
    std::cout << data;
    std::cin >> readVar;
    return readVar;
  }
#else
#endif
};
