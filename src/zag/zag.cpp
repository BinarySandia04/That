#include "zag.h"

#include "libs/termcolor.hpp"
#include <iostream>

using namespace Zag;

Error::Error(int position, std::string content) {
  this->position = position;
  this->content = content;
}

void Error::Print() {
  std::cout << termcolor::red << "ERROR AT POS " << position;
  std::cout << " " << content << termcolor::reset << std::endl;
}
