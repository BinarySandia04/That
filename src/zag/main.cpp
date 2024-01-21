#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "main.h"
#include "scanner.h"
#include "zag.h"

#include "libs/termcolor.hpp"
#include "libs/toml.hpp"

using namespace Zag;

void RunFile(std::string name);
void RunShell();
void Run(std::string code);

int main(int argc, char *argv[]) {
  if (argc > 2) {
    std::cout << termcolor::red << "Usage: zag <script>" << termcolor::reset;
  } else if (argc == 2) {
    try {
      RunFile(std::string(argv[1]));
    } catch (Error error) {
      error.Print();
    }
  } else {
    std::cout << termcolor::cyan << "Zag shell" << termcolor::reset
              << std::endl;
    RunShell();
  }
}

void RunFile(std::string name) {
  if (std::filesystem::exists(name)) {
    std::ifstream file;
    std::string code = "";
    std::string buffer;
    file.open(name);

    if (file.is_open()) {
      while (std::getline(file, buffer)) {
        code += buffer + "\n";
      }

      try {
        Run(code);
      } catch (Error error) {
        throw(error);
      }
    } else {
      throw(std::string("Error opening file " + name));
      return;
    }
  } else {
    throw(std::string("The file " + name + " doesn't exists"));
    return;
  }
}

void RunShell() {
  std::string line;
  std::cout << termcolor::green << "> " << termcolor::reset;
  std::getline(std::cin, line);
  do {
    try {
      Run(line);
    } catch (Error error) {
      error.Print();
    }

    std::cout << termcolor::green << "> " << termcolor::reset;
    std::string line;

  } while (std::getline(std::cin, line));
}

void Run(std::string code) {
  Scanner scanner = Scanner(code);
  std::vector<Token> tokens;
  scanner.ScanTokens(&tokens);

  for (int i = 0; i < tokens.size(); i++) {
    std::cout << termcolor::yellow << tokens[i].ToString() << termcolor::reset
              << std::endl;
  }
}
