#include "environment.h"

#include "termcolor.h"

using namespace ZagCXX;


void Scope::Delete(){
  for(auto &p : data){
    // std::cout << termcolor::yellow << p.first << termcolor::reset << ": ";
    if(p.second != nullptr){
      /*
      std::cout << "Deleted ";
      p.second->Print();
      */
      delete p.second;
    }
  }
}

void Scope::Print(){
  for(auto &p : data){
    std::cout << termcolor::yellow << p.first << termcolor::reset << ": ";
    if(p.second != nullptr) p.second->Print(0);
    else std::cout << "nullptr" << std::endl;
  }
}

