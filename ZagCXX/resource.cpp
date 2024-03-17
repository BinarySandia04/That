#include "resource.h"

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <string>
#include <cinttypes>

#include "termcolor/termcolor.hpp"

void Resource::RunScript(){
  std::stringstream ss(std::string(data(), size()));
  std::string command;

  while(std::getline(ss, command, '\n')){
    int err = std::system(command.c_str());
    if(err){
      std::cout << termcolor::red << "Error running script" << std::endl;
      return;
    }
  }

  return;
}

std::string Resource::string(){
  return std::string(mData, mSize);
}
