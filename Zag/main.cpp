#include "main.h"

#include "src/resource.h"

#include <iostream>

#include "argh.h"
#include "termcolor.h"
#include <ZagIR/Libs/packages.h>

using namespace ZagIR;
using namespace Zag;


int main(int argc, char *argv[]) {

  Resource test2 = LOAD_RESOURCE(folder_test2_txt);
  std::cout << std::string(test2.data(), test2.size()) << std::endl;

  auto cmdl = argh::parser(argc, argv);
  if (cmdl[1] == "init"){
    if(cmdl[2] == "lib"){
      InitLib();
    } else {
      InitProject();
    }
  } else {

    std::cout << "Hello Zag!" << std::endl;
    std::vector<ZagIR::Package> packages = ZagIR::FetchPackages();
    std::cout << "Found " << packages.size() << " packages" << std::endl;

    for (int i = 0; i < packages.size(); i++) {
      std::cout << "- " << packages[i].name << " [" << packages[i].version
                << "]: " << packages[i].packMap.size() << " bindings"
                << std::endl;
      for (auto &p : packages[i].packMap) {
        std::cout << "\t" << termcolor::color<122, 122, 122> << p.first
                  << termcolor::reset << std::endl;
      }
    }
  }
}

void Zag::InitProject(){
  std::cout << "init project" << std::endl;
}

void Zag::InitLib(){
  std::cout << "init lib" << std::endl;
}
