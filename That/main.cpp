#include "main.h"

#include "src/resource.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "argh/argh.h"
#include "termcolor/termcolor.hpp"

#include <ThatLib/Libs/packages.h>
#include <ThatLib/Logs/logs.h>
#include <ThatLib/Utils/system.h>
#include <ThatLib/Utils/thatpath.h>

using namespace ThatLib;
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

  auto cmdl = argh::parser(argc, argv);
  if (cmdl[1] == "init" || cmdl[1] == "i") {
    if (cmdl[2] == "package" || cmdl[2] == "p") {
      InitPackage();
    } else {
      InitProject();
    }
  } else if (cmdl[1] == "package" || cmdl[1] == "p") {
    if (cmdl[2] != "") {
      try {
        Package *pack = ThatLib::FetchPackage(cmdl[2]);
        if (cmdl[3] == "binds" || cmdl[3] == "b") {
          ShowBinds(pack);
        } else {
          PrintPackageInfo(pack);
        }
      } catch (const std::runtime_error &err) {
        Logs::Error(err.what());
      }
    } else {

      std::vector<ThatLib::Package *> packages;
      ThatLib::FetchPackages(&packages);
      for (int i = 0; i < packages.size(); i++) {
        PrintPackage(packages[i]);
      }

      for (int i = 0; i < packages.size(); i++) {
        delete packages[i];
      }
    }
  } else {
    Logs::Gradient("That programming language", Logs::Color(92, 145, 230),
                   Logs::Color(187, 28, 255));

    std::vector<ThatLib::Package *> packages;
    ThatLib::FetchPackages(&packages);

    Logs::Success("Found " + std::to_string(packages.size()) + " packages");

    for (int i = 0; i < packages.size(); i++) {
      delete packages[i];
    }
  }
}

void InitProject() { Logs::Info("Init project"); }

void InitPackage() {
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
    // TODO
  }
}

void PrintPackageInfo(ThatLib::Package *package) {
  PrintPackage(package);

  std::string currentSubpackage = "__________________";
  for (int i = 0; i < package->binds.size(); i++) {
    Binding *bind = package->binds[i];
    if (currentSubpackage != bind->subpackage) {
      currentSubpackage = bind->subpackage;
      if (currentSubpackage == "")
        std::cout << termcolor::bold << termcolor::cyan << "base"
                  << termcolor::reset << ":" << std::endl;
      else
        std::cout << termcolor::bold << currentSubpackage << termcolor::reset
                  << ":" << std::endl;
    }

    CFunction *cfunc = dynamic_cast<CFunction *>(bind);
    CType *ctype = dynamic_cast<CType *>(bind);
    Conversion *conversion = dynamic_cast<Conversion *>(bind);
    COperation *coperation = dynamic_cast<COperation *>(bind);

    if (cfunc != nullptr) {
      PrintBindStatus("F", bind);

      std::cout << cfunc->bind << ": " << cfunc->name;
      std::cout << "(";
      for (int i = 0; i < cfunc->funcArgs.size(); i++) {
        std::cout << cfunc->funcArgs[i];
        if (i < cfunc->funcArgs.size() - 1)
          std::cout << ", ";
      }
      std::cout << ") -> ";
      std::cout << cfunc->retType;
    } else if (ctype != nullptr) {
      PrintBindStatus("T", bind);
      std::cout << ctype->name << " ~> " << ctype->parent;

    } else if (conversion != nullptr) {
      if (conversion->implicit) {
        PrintBindStatus("c", bind);
        std::cout << conversion->fromType << " => " << conversion->toType;
      } else {
        PrintBindStatus("C", bind);
        std::cout << conversion->fromType << " => " << conversion->toType;

        std::cout << " (" << conversion->bind << ")";
      }
    } else if (coperation != nullptr) {
      if (coperation->implicit) {
        PrintBindStatus("o", bind);
        std::cout << coperation->lType << " " << coperation->op << " "
                  << coperation->rType << " => " << coperation->resType;
      } else {
        PrintBindStatus("O", bind);
        std::cout << coperation->lType << " " << coperation->op << " "
                  << coperation->rType << " => " << coperation->resType;
        std::cout << " (" << coperation->bind << ")";
      }
    }

    std::cout << std::endl;
  }
}

void PrintBindStatus(std::string letter, Binding *bind) {
  std::cout << "   " << termcolor::bold;
  std::cout << termcolor::yellow << letter << termcolor::reset << " ";
}

void ShowBinds(Package *pack) {
  fs::path path = pack->packInfo.path;
  std::cout << path.string() << std::endl;
  std::vector<std::string> mangles, demangles;
  std::string nmm, nmdm;
  nmm = ThatLib::Utils::Exec("nm -gDjUv " +
                           (path / ("lib" + pack->packInfo.name + ".so")).string());
  nmdm = ThatLib::Utils::Exec("nm -gDjUvC " +
                            (path / ("lib" + pack->packInfo.name + ".so")).string());

  auto nmmss = std::stringstream(nmm);
  auto nmdmss = std::stringstream(nmdm);

  for (std::string line; std::getline(nmmss, line, '\n');)
    mangles.push_back(line);
  for (std::string line; std::getline(nmdmss, line, '\n');)
    demangles.push_back(line);

  for (int i = 0; i < mangles.size(); i++) {
    std::cout << demangles[i] << " => " << mangles[i] << std::endl;
  }
}

void PrintPackage(ThatLib::Package *package) {
  std::cout << termcolor::bold << package->packInfo.name << termcolor::reset
            << termcolor::color<255, 128, 0> << " [" << termcolor::reset
            << package->packInfo.version << termcolor::color<255, 128, 0> << "]"
            << termcolor::reset << ": ";

  std::cout << termcolor::bold << package->binds.size() << termcolor::reset
            << " bindings";
  std::cout << termcolor::reset;
  std::cout << std::endl;
}
