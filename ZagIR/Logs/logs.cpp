#include "logs.h"

#include <stdint.h>
#include "termcolor.h"
// Small hack will comment later
#define TERMCOLOR_USE_ANSI_ESCAPE_SEQUENCES

ZagIR::Logs::Color::Color(uint8_t r, uint8_t g, uint8_t b){
  this->r = r;
  this->g = g;
  this->b = b;
}

void ZagIR::Logs::Print(std::string msg){
  std::cout << termcolor::white << msg << termcolor::reset << std::endl;
}

void ZagIR::Logs::Error(std::string msg){
  std::cout << termcolor::red << msg << termcolor::reset << std::endl;
}

void ZagIR::Logs::Success(std::string msg){
  std::cout << termcolor::green << msg << termcolor::reset << std::endl;
}

void ZagIR::Logs::Warning(std::string msg){
  std::cout << termcolor::yellow << msg << termcolor::reset << std::endl;
}

void ZagIR::Logs::Info(std::string msg){
  std::cout << termcolor::cyan << msg << termcolor::reset << std::endl;
}

std::string ZagIR::Logs::Ask(std::string question){
  std::cout << termcolor::white << question << termcolor::reset;
  std::string res;
  std::cin >> res;
  return res;
}

void ZagIR::Logs::Gradient(std::string text, Color c1, Color c2){
  std::string res = "";
  Color c = c1;
  float t = 0;
  for(int i = 0; i < text.size(); i++, t += 1. / text.size()){
    c.r = c1.r + (c2.r - c1.r) * t;
    c.g = c1.g + (c2.g - c1.g) * t;
    c.b = c1.b + (c2.b - c1.b) * t;
    res += color(c) + text[i];
  }
  std::cout << res << termcolor::reset << std::endl;
}


std::string ZagIR::Logs::color(Color c) {
  std::string res = "";
#if defined(TERMCOLOR_USE_ANSI_ESCAPE_SEQUENCES)
    res += std::string("\033[38;2;") + std::to_string((int) c.r) + ";" + std::to_string((int) c.g) + ";" + std::to_string((int) c.b) + "m";
#elif defined(TERMCOLOR_USE_WINDOWS_API)
#endif
  return res;
}
