#include "main.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#ifdef __linux__
#include <pwd.h>
#include <sys/types.h>
#endif

#include "argh.h"
#include "termcolor.h"
#include "toml.h"

#include "ZagIR/Ast/ast.h"

#include "Transpiler/transpiler.h"
#include "flags.h"

int main(int argc, char *argv[]) {

  auto cmdl = argh::parser(argc, argv);

  if (cmdl[{"-d", "--debug"}]) {
    programFlags = programFlags | Flags::DEBUG;
  }

  if(cmdl[{"-f", "--force"}]){
    programFlags = programFlags | Flags::FORCE;
  }

  if (cmdl.size() > 1) {
    for (int i = 1; i < cmdl.size(); i++) {
      try {
        TranspileFile(cmdl.pos_args()[i]);
      } catch (std::string err) {
        std::cout << termcolor::red << err << termcolor::reset << std::endl;
      }
    }
  }

  if (cmdl.size() == 1) {
    std::cout << "Zag" << std::endl;
  }
  return 0;
}

void TranspileFile(std::string fileName) {
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

void Transpile(std::string code, std::string fileName) {
  ZagIR::Node *ast = new ZagIR::Node();
  ZagIR::GenerateAst(code, fileName, ast, programFlags & Flags::DEBUG);

  ZagCXX::Transpiler transpiler;

#ifdef __linux
  std::filesystem::path homePath =
      std::filesystem::path(getenv("HOME")) / std::filesystem::path(".zag");
#elif _WIN32
  std::filesystem::path homePath(getenv("USERPROFILE"));
#endif


  if (!std::filesystem::is_directory(homePath) ||
      !std::filesystem::exists(homePath)) {      // Check if src folder exists
    std::filesystem::create_directory(homePath); // create src folder
    std::cout << termcolor::yellow << "Created env directory" << termcolor::reset << std::endl;
  }

  std::filesystem::path tmpSourcePath(homePath /
                                      std::filesystem::path("_tmp.cpp"));
  std::filesystem::path tmpOutPath(homePath / std::filesystem::path("_tmp"));

  std::string transCode, cxxargs;
  transCode = transpiler.GenerateSource(ast, &cxxargs);

  if(programFlags & DEBUG){
    std::cout << termcolor::green << "CODE: " << termcolor::reset << std::endl << transCode << std::endl;
  }

  std::ifstream tmpSourceIn(tmpSourcePath.string());
  std::stringstream buffer;
  buffer << tmpSourceIn.rdbuf();
  std::string lastCode = buffer.str();

  if (lastCode != transCode || !std::filesystem::exists(tmpOutPath) || programFlags & FORCE) {

    std::ofstream tmpSourceOut(tmpSourcePath.string());
    tmpSourceOut << transCode;
    tmpSourceOut.close();
    delete ast;

    if (Compile(tmpSourcePath, tmpOutPath, cxxargs)) {
      std::cout << termcolor::red << "Error compiling" << termcolor::reset
                << std::endl;
      std::filesystem::remove(tmpSourcePath);
      return;
    }
  }
  Run(tmpOutPath);
}

int Compile(std::filesystem::path sourcePath, std::filesystem::path outPath, std::string cxxargs) {
  std::string compileRun = "g++ " + cxxargs + " " + sourcePath.string() + " -o " + outPath.string();
  if(programFlags & DEBUG) std::cout << termcolor::red << compileRun << termcolor::reset << std::endl;
  return system(compileRun.c_str());
}

void Run(std::filesystem::path outPath) { system(outPath.string().c_str()); }
