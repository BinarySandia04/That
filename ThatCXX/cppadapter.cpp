#include "cppadapter.h"

#include <string>
#include <iostream>
#include <stack>

using namespace ThatCXX;



CppAdapter::CppAdapter(){
  res = "";
}

std::string CppAdapter::GetResult(){
  return res;
}

void CppAdapter::WriteToExpStack(std::string data){
  res += data;
  if(opStack.size() > 0){
    res += opStack.top();
    opStack.pop();
  }

}

// -----

void CppAdapter::OpenBlock(){
  res += "{";
}

void CppAdapter::CloseBlock(){
  res += "}";
}

void CppAdapter::Int(std::string data){
  WriteToExpStack(data);
}

void CppAdapter::Num(std::string data){
  WriteToExpStack(data);
}

void CppAdapter::Yep(){
  WriteToExpStack("true");
}

void CppAdapter::Nop(){
  WriteToExpStack("false");
}

void CppAdapter::Str(std::string data){
  WriteToExpStack("std::string(\"" + data + "\")");
}

void CppAdapter::Chr(std::string data){
  WriteToExpStack("'" + data + "'");
}

void CppAdapter::Id(std::string data) {
  // La referencia d'un identifier es tal cual el seu nom
  WriteToExpStack(data);
}

void CppAdapter::Binary(std::string op) {
  // Aqui es més complicat, hauriem d'haver passat dos
  // crides a expressions a priori. Podriem declarar en aquesta
  // classe un stack potser?
  opStack.push(op);
}
