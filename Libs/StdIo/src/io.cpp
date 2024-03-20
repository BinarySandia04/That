#include "io.h"

#include <iostream>
#include <string>

int zstd::ReadInt(std::string s){
  int n;
  Print(s);
  std::cin >> n;
  return n;
}

std::string zstd::ReadStr(std::string s){
  std::string r;
  Print(s);
  std::getline(std::cin, r);
  return r;
}


