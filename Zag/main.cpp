#include <iostream>

#include <ZagIR/Libs/packages.h>
#include "termcolor.h"

using namespace ZagIR;

int main() {
  std::cout << "Hello Zag!" << std::endl;
  std::vector<ZagIR::Package> packages = ZagIR::FetchPackages();
  std::cout << "Found " << packages.size() << " packages" << std::endl;

  for (int i = 0; i < packages.size(); i++) {
    std::cout << "- " << packages[i].name << " [" << packages[i].version << "]: "
              << packages[i].packMap.size() << " bindings" << std::endl;
    for(auto &p : packages[i].packMap){
      std::cout << "\t" << termcolor::color<122,122,122> << p.first << termcolor::reset << std::endl;
    }

  }
}
