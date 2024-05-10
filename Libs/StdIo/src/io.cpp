#include "io.h"

#include <iostream>
#include <string>

int tstd::ReadInt(std::string s){
  int n;
  Print(s);
  std::cin >> n;
  return n;
}

std::string tstd::ReadStr(std::string s){
  std::string r;
  Print(s);
  std::getline(std::cin, r);
  return r;
}

double tstd::ReadNum(std::string s){
  double d;
  Print(s);
  std::cin >> d;
  return d;
}

void tstd::PrintChar(int n){
  std::cout << (char) n;
}

void tstd::PrintNum(double d, int p){
  std::cout << std::setprecision(p) << d;
}
