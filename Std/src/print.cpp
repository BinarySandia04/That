// Libs que fa servir el copypaste
#include <iostream>

// _ZAGCXX definit significa que aixo fa copy paste al transpiler
// si no està definit el que fa es fer extern "C" i va al IR per altres
// coses intermitges
#ifndef _ZAGCXX
#include <string>
#include "ZagIR/Libs/Types.h"
#endif

namespace zagstd {
#ifdef _ZAGCXX
  template <typename T>
  void print(T data){
    std::cout << data << std::endl;
  }
#else
  extern "C" void print(ZagIR::Data* data){
    if(data->type == "Int"){
      std::cout << *(int*)(data->value);
    } else if(data->type == "Str"){
      std::cout << *(std::string*)(data->value);
    } else {
      // Suposo que data->data es d'una classe més complexa o alguna cosa així
    }
  }
#endif
};
