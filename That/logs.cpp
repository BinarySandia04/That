#include "logs.h"

#include <iostream>
#include <stdint.h>
#include <stc/stc.hpp>
#include <string>
#include <cstdarg>

using namespace That;

void Logs::Print(std::string msg){
  std::cout << stc::reset << msg << stc::reset << std::endl;
}

void Logs::Error(std::string msg){
  std::cout << stc::rgb_fg(255, 0, 0) << msg << stc::reset << std::endl;
}

void Logs::Success(std::string msg){
  std::cout << stc::rgb_fg(0,255,0) << msg << stc::reset << std::endl;
}

void Logs::Warning(std::string msg){
  std::cout << stc::rgb_fg(255,255,0) << msg << stc::reset << std::endl;
}

void Logs::Info(std::string msg){
  std::cout << stc::rgb_fg(0,255,255) << msg << stc::reset << std::endl;
}

void Logs::Debug(std::string msg){
  std::cout << stc::rgb_fg(188,188,188) << "[DEBUG] " << stc::reset << msg << std::endl;
}

void Logs::Printf(const char* fmt, ...){
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);   // forward to standard printf
  va_end(args);
}

std::string Logs::Ask(std::string question){
  std::cout << stc::reset << question << stc::reset;
  std::string res;
  std::cin >> res;
  return res;
}

