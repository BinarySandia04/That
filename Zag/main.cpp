#include "main.h"

#include "src/resource.h"

#include <filesystem>
#include <iostream>

#include "argh.h"
#include "termcolor.h"

#include <ZagIR/Libs/packages.h>
#include <ZagIR/Logs/logs.h>

using namespace ZagIR;
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

  auto cmdl = argh::parser(argc, argv);
  if (cmdl[1] == "init") {
    if (cmdl[2] == "lib") {
      InitLib();
    } else {
      InitProject();
    }
  } else {
    Logs::Gradient("Zag programming language", Logs::Color(92, 145, 230),
                   Logs::Color(187, 28, 255));

    std::vector<ZagIR::Package*> packages;
    ZagIR::FetchPackages(&packages);

    Logs::Success("Found " + std::to_string(packages.size()) + " packages");

    if (cmdl[{"--list-packages"}]) {
      for (int i = 0; i < packages.size(); i++) {
        PrintPackageInfo(packages[i]);
      }
    }

    for(int i = 0; i < packages.size(); i++){
      delete packages[i];
    }
  }
}

void InitProject() { Logs::Info("Init project"); }

void InitLib() {
  Logs::Info("Creating lib inside the current directory");
  std::string libName = Logs::Ask("Name? ");
  std::string author = Logs::Ask("Author? ");
  std::vector<Resource> resources = {
      LOAD_RESOURCE(Lib_package_toml, "package.toml"),
      LOAD_RESOURCE(Lib_README_md, "README.md"),
      LOAD_RESOURCE(Lib_CMakeLists_txt, "CMakeLists.txt"),
  };

  WriteFiles(resources);
}

void WriteFiles(std::vector<Resource> res) {
  for (int i = 0; i < res.size(); i++) {
    fs::path currentPath = fs::current_path();
    std::cout << currentPath.string() << std::endl;
  }
}

void PrintPackageInfo(ZagIR::Package* package) {
  std::cout << termcolor::bold << package->name << termcolor::reset
            << termcolor::color<255, 128, 0> << " [" << termcolor::reset
            << package->version << termcolor::color<255, 128, 0> << "]"
            << termcolor::reset << ": ";

  std::cout << termcolor::bold << package->functionMap.size() << termcolor::reset
            << " bindings, ";
  std::cout << termcolor::bold << package->typeMap.size() << termcolor::reset
            << " types";
  std::cout << std::endl;

  for (auto &p : package->functionMap) {
    FunctionCall* call = p.second;
    std::cout << "   " << termcolor::bold << "F " << termcolor::reset
              << call->bind << ": " << p.first;
    std::cout << "(";
    for (int i = 0; i < call->funcArgs.size(); i++) {
      std::cout << call->funcArgs[i];
      if (i < call->funcArgs.size() - 1)
        std::cout << ", ";
    }
    std::cout << ") -> ";
    std::cout << call->retType;
    std::cout << std::endl;
  }

  for (auto &p : package->typeMap) {
    ImportType* type = p.second;
    std::cout << "   " << termcolor::bold << "T " << termcolor::reset << p.first
              << " ~> " << type->parent;
    std::cout << std::endl;
  }

  for(int i = 0; i < package->conversionMap.size(); i++){
    Conversion* conversion = package->conversionMap[i];
    std::cout << "   " << termcolor::bold << "C " << termcolor::reset << conversion->lType << " => " << conversion->rType << std::endl;
  }
}
