#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "error.h"
#include "flags.h"
#include "main.h"
#include "parser.h"
#include "scanner.h"

#include "libs/argh.h"
#include "libs/termcolor.hpp"
#include "libs/toml.hpp"

using namespace Zag;

void RunFile(std::string name);
void RunShell();
void Run(std::string code);

int main(int argc, char *argv[]) {

  auto cmdl = argh::parser(argc, argv);

  if (cmdl[{"-d", "--debug"}]) {
    programFlags = programFlags | Flags::DEBUG;
  }

  if (cmdl.size() > 1) {
    for (int i = 1; i < cmdl.size(); i++) {
      RunFile(cmdl.pos_args()[i]);
    }
  }

  if (cmdl.size() == 1)
    RunShell();
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
        Run(code, name);
      } catch (Error error) {
        error.Print(code);
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
      Run(line, "");
    } catch (Error error) {
      error.Print(line);
    }

    std::cout << termcolor::green << "> " << termcolor::reset;
    std::string line;

  } while (std::getline(std::cin, line));
}

void Run(std::string code, std::string fileName) {

  Error error;

  Scanner scanner = Scanner(code, fileName);
  std::vector<Token> tokens;

  if (!scanner.ScanTokens(&tokens, &error)) {
    error.Print(code);
    return;
  }

  if (programFlags & Flags::DEBUG) {
    for (int i = 0; i < tokens.size(); i++) {
      std::cout << termcolor::yellow << tokens[i].ToString() << termcolor::reset
                << std::endl;
    }
  }

  Parser parser(fileName);
  Node *ast = new Node(NODE_SPACE);

  if (!parser.GenerateAST(&tokens, &ast, &error)) {
    error.Print(code);
    return;
  }

  if(programFlags & Flags::DEBUG) ast->Debug(0);
}
