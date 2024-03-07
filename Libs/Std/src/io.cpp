#include "io.h"

#include <iostream>
#include <string>

void io::Print(std::string s){
  std::cout << s;
}

void io::PrintLn(std::string s){
  std::cout << s << std::endl;
}

int io::ReadInt(std::string s){
  int n;
  Print(s);
  std::cin >> n;
  return n;
}

std::string io::ReadStr(std::string s){
  std::string r;
  Print(s);
  std::getline(std::cin, r);
  return r;
}
