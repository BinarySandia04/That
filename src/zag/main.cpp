#include <filesystem>
#include <iostream>
#include <string>

#include "libs/termcolor.hpp"
#include "libs/toml.hpp"
#include "version-g.h"

void RunFile(std::string name);
void RunShell();

int main(int argc, char *argv[]) {
  if (argc > 2) {
    std::cout << termcolor::red << "Usage: zag <script>" << termcolor::reset;
  } else if (argc == 2) {
    try {
      RunFile(std::string(argv[1]));
    } catch (std::string error) {
      std::cout << termcolor::red << error << std::endl;
    }
  } else {
    std::cout << termcolor::cyan << "About " << VERSION << termcolor::reset
              << std::endl;
  }
}

void RunFile(std::string name) {
  if (std::filesystem::exists(name)) {
    std::cout << "Existe" << std::endl;
  } else {
    throw(std::string("The file "));
  }
}

void RunShell() {}
