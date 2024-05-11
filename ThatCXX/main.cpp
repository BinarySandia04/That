#include "main.h"

#include <ThatLib/Logs/logs.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "argh/argh.h"
#include "resource.h"
#include "termcolor/termcolor.hpp"
#include "toml++/toml.hpp"

#include <ThatLib/Ast/ast.h>
#include <ThatLib/Utils/thatpath.h>

#include "Formatter/formatter.h"
#include "Transpiler/transpiler.h"
#include "flags.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

  auto cmdl = argh::parser(argc, argv);

  if (cmdl[{"-d", "--debug"}]) {
    programFlags = programFlags | Flags::DEBUG;
  }
  
  if (cmdl[{"-f", "--force"}]) {
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
    std::cout << "That" << std::endl;
  }
  return 0;
}

void TranspileFile(std::string fileName) {
  if (fs::exists(fileName)) {
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
      } catch (ThatLib::Error error) {
        error.Print(code);
      }

      file.close();
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
  ThatLib::Node *ast = new ThatLib::Node();
  ThatLib::GenerateAst(code, fileName, ast, programFlags & Flags::DEBUG);

  ThatCXX::Transpiler transpiler;

#ifdef __linux
  fs::path homePath = fs::path(getenv("HOME")) / fs::path(".that");
#elif _WIN32
  std::filesystem::path homePath(getenv("USERPROFILE"));
#endif

  if (!fs::is_directory(homePath) ||
      !fs::exists(homePath)) {      // Check if src folder exists
    fs::create_directory(homePath); // create src folder
    std::cout << termcolor::yellow << "Created env directory"
              << termcolor::reset << std::endl;
  }

  // De moment serà _tmp, dependrà del projecte
  fs::path tmpSourcePath(homePath / "_tmp");

  // La regenerem cada cop per debugging
  if (fs::exists(tmpSourcePath) && programFlags & DEBUG) {
    fs::remove_all(tmpSourcePath);
  }
  if (!fs::is_directory(tmpSourcePath) || !fs::exists(tmpSourcePath)) {
    fs::create_directory(tmpSourcePath);
  }

  if (!fs::exists(tmpSourcePath / "src")) {
    fs::create_directory(tmpSourcePath / "src");
  }

  fs::path tmpSourceMainPath(tmpSourcePath / "main.cpp");
  // fs::path tmpOutPath(homePath / std::filesystem::path("_tmp"));

  std::string transCode, cxxargs;
  std::vector<std::string> libNames;
  transCode = transpiler.GenerateSource(ast, &cxxargs, &libNames);
  delete ast;

  // We populate our source with the libs
  for (int i = 0; i < libNames.size(); i++) {
    fs::path thatUsrLib(THAT_PATH);
    fs::copy(homePath / "sources" / libNames[i] / "src",
             tmpSourcePath / "src" / libNames[i],
             fs::copy_options::overwrite_existing |
                 fs::copy_options::recursive);
  }

  if (programFlags & DEBUG) {
    std::cout << termcolor::green << "CODE: " << termcolor::reset << std::endl
              << transCode << std::endl;
  }

  std::ifstream tmpSourceIn(tmpSourceMainPath.string());
  std::stringstream buffer;
  buffer << tmpSourceIn.rdbuf();
  std::string lastCode = buffer.str();

  if (lastCode != transCode || !fs::exists(tmpSourceMainPath) ||
      programFlags & FORCE) {

    std::ofstream tmpSourceOut(tmpSourceMainPath.string());
    tmpSourceOut << transCode;
    tmpSourceOut.close();

    // Generate CMakeFile.txt
    Resource res = LOAD_RESOURCE(CMakeLists_txt, "CMakeLists.txt");
    ThatCXX::Formatter formatter;
    std::string cMakeListsContent =
        formatter.Format(res.string(), {{1, "Project"}, {2, ""}});

    std::ofstream tmpCMakeListsOut(tmpSourcePath / "CMakeLists.txt");
    tmpCMakeListsOut << cMakeListsContent;
    tmpCMakeListsOut.close();

    CompileCMake(tmpSourcePath);
    /*
    if (Compile(tmpSourcePath, tmpOutPath, cxxargs)) {
      std::cout << termcolor::red << "Error compiling" << termcolor::reset
                << std::endl;
      fs::remove(tmpSourcePath);
      return;
    }
    */
  }
  Logs::Success("Running...");
  Run(tmpSourcePath / ".build" / "Project");
  // Run(tmpOutPath);
}

// TODO: Windows???
int CompileCMake(fs::path sourcePath) {
  if (!fs::exists(sourcePath / ".build") ||
      !fs::is_directory(sourcePath / ".build")) {
    fs::create_directory(sourcePath / ".build");
  }

  fs::current_path(sourcePath / ".build");
  if (programFlags & DEBUG) {
    system("cmake ../");
    system("cmake --build .");
  } else {
    Logs::Info("Compiling...");
    system("cmake ../ >/dev/null");
    system("cmake --build . >/dev/null");
  }
  return 0;
}

/*
int Compile(fs::path sourcePath, fs::path outPath, std::string cxxargs) {
  std::string compileRun = "g++ " + cxxargs + " " + sourcePath.string() + " -o "
+ outPath.string(); if(programFlags & DEBUG) std::cout << termcolor::red <<
compileRun << termcolor::reset << std::endl; return system(compileRun.c_str());
}
*/

void Run(fs::path outPath) {

  if (programFlags & DEBUG)
    std::cout << termcolor::yellow
              << "------------------ Exec -----------------------"
              << termcolor::reset << std::endl;
  system(outPath.string().c_str());
}
