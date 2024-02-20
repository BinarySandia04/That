#include "environment.h"

#include <iostream>
#include <string>

using namespace ZagIR;
using namespace ZagCXX;

Object::Object() { objType = OBJECT_EMPTY; }

Object::Object(std::string type) {
  objType = OBJECT_VARIABLE;
  this->varData = type;
}

Object::Object(ZagIR::PackCall cfunctionData) {
  objType = OBJECT_CFUNCTION;
  this->cfunctionData = cfunctionData;
}

void Object::AddChild(Object obj, std::string path) {
  if (!(objType == OBJECT_CONTAINER || objType == OBJECT_CCONTAINER)) {
    // TODO: Change to exception
    std::cout << "Tried to call AddChild in object that is not "
                 "(OBJECT_CONTAINER | OBJECT_CCONTAINER)"
              << std::endl;
    return;
  }

  std::string firstPart = "", secondPart = "";

  // Populate firstPart and secondPart
  for (int i = 0; i < path.size(); i++) {
    if (path[i] != '.') {
      firstPart += path[i];
    } else {
      i++;
      while (i < path.size()) {
        secondPart += path[i];
        i++;
      }
    }
  }

  if (containerData.find(firstPart) == containerData.end()) {
    containerData[firstPart] = obj;
  } else {
    containerData[firstPart].AddChild(obj, secondPart);
  }
}

Object *Object::Get(std::string key) { return &containerData[key]; }

ZagIR::PackCall Object::GetCFunctionData() { return cfunctionData; }

void Object::Print() {
  if (objType == OBJECT_EMPTY) {
    std::cout << "OBJECT_EMPTY" << std::endl;
  } else if (objType == OBJECT_VARIABLE) {
    std::cout << "OBJECT_VARIABLE" << std::endl;
  } else if (objType == OBJECT_CONTAINER) {
    std::cout << "OBJECT_CONTAINER" << std::endl;
  } else if (objType == OBJECT_FUNCTION) {
    std::cout << "OBJECT_FUNCTION" << std::endl;
  } else if (objType == OBJECT_CFUNCTION) {
    std::cout << "OBJECT_CFUNCTION: " << cfunctionData.funcName << std::endl;
  } else if (objType == OBJECT_CCONTAINER) {
    std::cout << "OBJECT_CCONTAINER" << std::endl;
    std::cout << "-----" << std::endl;
    for(auto &p : containerData){
      std::cout << p.first << std::endl;
      p.second.Print();
    }
    std::cout << "-----" << std::endl;
  }
}
