#include "resource.h"

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <string>

#include <That/logs.h>

using namespace That;

void Resource::RunScript(){
  std::stringstream ss(std::string(data(), size()));
  std::string command;

  while(std::getline(ss, command, '\n')){
    int err = std::system(command.c_str());
    if(err){
      Logs::Error("Error running script");
      return;
    }
  }

  return;
}
