#include "vartype.h"

using namespace ZagCXX;

VarType::VarType() {
  this->name = "Nil";
  this->translation = "void";
  this->includes = "";
}

VarType::VarType(std::string name, std::string translation,
                 std::string includes) {
  this->name = name;
  this->translation = translation;
  this->includes = includes;
}

std::string VarType::Transpile() {
  std::string formatted = "";

  for (int i = 0; i < translation.size(); i++) {
    if (i < translation.size() - 1) {
      switch (translation[i]) {
      case '{':
        i++;
        if (translation[i + 1] != '{') {
          formatted += TranspileChildren();
        }
        break;
      case '}':
        if (translation[i + 1] == '}') {
          i++;
        }
        break;
      default:
        break;
      }
    }
    formatted += translation[i];
  }

  return formatted;
}

std::string VarType::TranspileChildren(){
  std::string res = "";
  for(int i = 0; i < children.size(); i++){
    res += children[i]->Transpile();
    if(i < children.size() - 1) res += ",";
  }
  return res;
}
