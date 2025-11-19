#include "logs.h"

#include <iostream>
#include <stdint.h>
#include <stc/stc.hpp>

void ThatLib::Logs::Print(std::string msg){
  std::cout << stc::reset << msg << stc::reset << std::endl;
}

void ThatLib::Logs::Error(std::string msg){
  std::cout << stc::rgb_fg(255, 0, 0) << msg << stc::reset << std::endl;
}

void ThatLib::Logs::Success(std::string msg){
  std::cout << stc::rgb_fg(0,255,0) << msg << stc::reset << std::endl;
}

void ThatLib::Logs::Warning(std::string msg){
  std::cout << stc::rgb_fg(255,255,0) << msg << stc::reset << std::endl;
}

void ThatLib::Logs::Info(std::string msg){
  std::cout << stc::rgb_fg(0,255,255) << msg << stc::reset << std::endl;
}

void ThatLib::Logs::Debug(std::string msg){
  std::cout << stc::rgb_fg(188,188,188) << "[DEBUG] " << stc::reset << msg << std::endl;
}

std::string ThatLib::Logs::Ask(std::string question){
  std::cout << stc::reset << question << stc::reset;
  std::string res;
  std::cin >> res;
  return res;
}


