#include "environment.h"

#include "termcolor.h"

using namespace ZagCXX;

void Scope::Print(){
  for(auto &p : data){
    std::cout << termcolor::yellow << p.first << termcolor::reset << ": ";
    if(p.second != nullptr) p.second->Print();
    else std::cout << "nullptr" << std::endl;
  }
}
