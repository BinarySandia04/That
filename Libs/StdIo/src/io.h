#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace zstd {

template <typename T>
void Print(T s){
  std::cout << s;
}

template <typename T>
void PrintLn(T s){
  std::cout << s << std::endl;
}

int ReadInt(std::string s);
double ReadNum(std::string s);
std::string ReadStr(std::string s);

}; // namespace io
