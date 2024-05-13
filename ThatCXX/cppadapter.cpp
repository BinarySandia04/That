#include "cppadapter.h"

#include <string>
#include <iostream>

using namespace ThatCXX;

CppAdapter::CppAdapter(){
  res = "";
}

std::string CppAdapter::GetResult(){
  return res;
}

/*
void CppAdapter::OpenBlock(){
  res += "{";
}

void CppAdapter::CloseBlock(){
  res += "}";
}

void CppAdapter::Int(std::string data){
  res += data;
}

void CppAdapter::Num(std::string data){
  res += data;
}

void CppAdapter::Yep(){
  res += "true";
}

void CppAdapter::Nop(){
  res += "false";
}

void CppAdapter::Str(std::string data){
  res += "std::string(\"" + data + "\")";
}

void CppAdapter::Chr(std::string data){
  res += "'" + data + "'";
}

void CppAdapter::Id(std::string data) {
  // La referencia d'un identifier es tal cual el seu nom
  res += data;
}

void CppAdapter::Binary(std::string op) {
  // Aqui es més complicat, hauriem d'haver passat dos
  // crides a expressions a priori. Podriem declarar en aquesta
  // classe un stack potser?
}
*/
