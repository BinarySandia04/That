#pragma once

#include <string>
#include <vector>

namespace zstd {
void Print(std::string s);
void PrintLn(std::string s);

int ReadInt(std::string s);
std::string ReadStr(std::string s);

template <typename T>
void PushArr(std::vector<T>& vec, T element){
  vec.push_back(element);
}

template <typename T>
int GetArrSize(std::vector<T>& vec){
  return vec.size();
}

}; // namespace io
