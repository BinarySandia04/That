#include "main.h"

#include <iostream>
#include <string>
#include <filesystem>

#include "argh.h"
#include "toml.h"
#include "termcolor.h"

#include "ZagIR/Ast/ast.h"

#include "flags.h"
#include "Transpiler/transpiler.h"

int main(int argc, char *argv[]) {

  auto cmdl = argh::parser(argc, argv);

  if (cmdl[{"-d", "--debug"}]) {
    programFlags = programFlags | Flags::DEBUG;
  }

  if (cmdl.size() > 1) {
    for (int i = 1; i < cmdl.size(); i++) {
      try {
        TranspileFile(cmdl.pos_args()[i]);
      } catch(std::string err){
        std::cout << termcolor::red << err << termcolor::reset << std::endl;
      }
    }
  }

  if (cmdl.size() == 1) {
    std::cout << "test" << std::endl;
  }
  return 0;
}

void TranspileFile(std::string fileName) {
  std::cout << "Transpliling file " << fileName << std::endl;

  if (std::filesystem::exists(fileName)) {
    std::ifstream file;
    std::string code = "";
    std::string buffer;
    file.open(fileName);

    if (file.is_open()) {
      while (std::getline(file, buffer)) {
        code += buffer + "\n";
      }

      try {
        Transpile(code, fileName);
      } catch (ZagIR::Error error) {
        error.Print(code);
      }
    } else {
      throw(std::string("Error opening file " + fileName));
      return;
    }
  } else {
    throw(std::string("The file " + fileName + " doesn't exists"));
    return;
  }
}

void Transpile(std::string code, std::string fileName){
  ZagIR::Node* ast = new ZagIR::Node();
  ZagIR::GenerateAst(code, fileName, ast, programFlags & Flags::DEBUG);

  ZagCXX::Transpiler transpiler;
  std::string out = transpiler.GenerateSource(ast);

  delete ast;
  std::cout << out << std::endl;
}

