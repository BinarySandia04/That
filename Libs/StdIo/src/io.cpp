#include "io.h"

#include <iostream>
#include <string>

void zstd::Print(std::string s){
  std::cout << s;
}

void zstd::PrintLn(std::string s){
  std::cout << s << std::endl;
}

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


