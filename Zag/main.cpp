#include "main.h"

#include "src/resource.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "argh.h"
#include "termcolor.h"

#include <ZagIR/Libs/packages.h>
#include <ZagIR/Logs/logs.h>
#include <ZagIR/Utils/system.h>

using namespace ZagIR;
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
        Package *pack = ZagIR::FetchPackage(cmdl[2]);
        if (cmdl[3] == "binds" || cmdl[3] == "b") {
          ShowBinds(pack);
        } else {
          PrintPackageInfo(pack);
        }
      } catch (const std::runtime_error &err) {
        Logs::Error(err.what());
      }
    } else {

      std::vector<ZagIR::Package *> packages;
      ZagIR::FetchPackages(&packages);
      for (int i = 0; i < packages.size(); i++) {
        PrintPackage(packages[i]);
      }

      for (int i = 0; i < packages.size(); i++) {
        delete packages[i];
      }
    }
  } else {
    Logs::Gradient("Zag programming language", Logs::Color(92, 145, 230),
                   Logs::Color(187, 28, 255));

    std::vector<ZagIR::Package *> packages;
    ZagIR::FetchPackages(&packages);

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
  }
}

void PrintPackageInfo(ZagIR::Package *package) {
  for (int j = 0; j < package->subpackages.size(); j++)
    package->LoadSubPackage(package->subpackages[j]);

  PrintPackage(package);

  bool ok = true;

  std::string currentSubpackage = "__________________";
  for (int i = 0; i < package->binds.size(); i++) {
    Binding *bind = package->binds[i];
    if(currentSubpackage != bind->subpackage){
      currentSubpackage = bind->subpackage;
      if(currentSubpackage == "") std::cout << termcolor::bold << termcolor::cyan << "base" << termcolor::reset << ":" << std::endl;
      else std::cout << termcolor::bold << currentSubpackage << termcolor::reset << ":" << std::endl; 
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
      ok = ok && cfunc->good;
    } else if (ctype != nullptr) {
      if (ctype->internal) {
        PrintBindStatus("t", bind);
        std::cout << ctype->name<< " ~> " << ctype->parent;
      } else {
        PrintBindStatus("T", bind);
        std::cout << ctype->name<< " ~> " << ctype->parent;
      }

    } else if (conversion != nullptr) {
      if (conversion->implicit) {
        PrintBindStatus("c", bind);
        std::cout << conversion->fromType << " => " << conversion->toType;
      } else {
        PrintBindStatus("C", bind);
        std::cout << conversion->fromType << " => " << conversion->toType;

        std::cout << " (" << conversion->bind << ")";
      }
      ok = ok && conversion->good;
    } else if(coperation != nullptr){
      if(coperation->implicit){
        PrintBindStatus("o", bind);
        std::cout << coperation->lType << " " << coperation->op << " " << coperation->rType << " => " << coperation->resType;
      } else {
        PrintBindStatus("O", bind);
        std::cout << coperation->lType << " " << coperation->op << " " << coperation->rType << " => " << coperation->resType;
        std::cout << " (" << coperation->bind << ")";
      }
    }

    if(bind->good) std::cout << " [" << bind->foundBind << "]";

    std::cout << std::endl;
  }

  // Print if valid
  if (ok)
    std::cout << termcolor::green << "All good" << termcolor::reset
              << std::endl;
}

void PrintBindStatus(std::string letter, Binding *bind) {
  std::cout << "   " << termcolor::bold;
  bool good = bind->good;

  if (good) {
    std::cout << termcolor::green;
  } else {
    if (bind->duped)
      std::cout << termcolor::yellow;
    else
      std::cout << termcolor::red;
  }

  std::cout << letter << termcolor::reset << " ";
}

void ShowBinds(Package *pack) {
  fs::path path = pack->path;
  std::cout << path.string() << std::endl;
  std::vector<std::string> mangles, demangles;
  std::string nmm, nmdm;
  nmm = ZagIR::Utils::Exec("nm -gDjUv " +
                           (path / ("lib" + pack->name + ".so")).string());
  nmdm = ZagIR::Utils::Exec("nm -gDjUvC " +
                            (path / ("lib" + pack->name + ".so")).string());

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

void PrintPackage(ZagIR::Package *package) {
  std::cout << termcolor::bold << package->name << termcolor::reset
            << termcolor::color<255, 128, 0> << " [" << termcolor::reset
            << package->version << termcolor::color<255, 128, 0> << "]"
            << termcolor::reset << ": ";

  std::cout << termcolor::bold << package->binds.size() << termcolor::reset
            << " bindings, ";
  std::cout << termcolor::bold << package->subpackages.size()
            << termcolor::reset << " subpackages";
  std::cout << termcolor::reset;
  std::cout << std::endl;
}
