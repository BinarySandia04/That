#include "environment.h"

#include <string>
#include <iostream>

using namespace ZagIR;
using namespace ZagCXX;

Object::Object() { objType = OBJECT_EMPTY; }

Object::Object(std::string type) {
  objType = OBJECT_VARIABLE;
  this->varData = type;
}

Object::Object(ZagIR::PackCall cfunctionData){
  objType = OBJECT_CFUNCTION;
  this->cfunctionData = cfunctionData;
}

void Object::AddChild(Object obj, std::string path){
  if(objType != OBJECT_CONTAINER){
    // TODO: Change to exception
    std::cout << "Tried to call AddChild in object that is not OBJECT_CONTAINER" << std::endl;
    return;
  }

  std::string firstPart = "", secondPart = "";
  
  // Populate firstPart and secondPart
  for(int i = 0; i < path.size(); i++){
    if(path[i] != '.'){
      firstPart += path[i];
    } else {
      i++;
      while(i < path.size()){
        secondPart += path[i];
        i++;
      }
    }
  }

  if(containerData.find(firstPart) == containerData.end()){
    containerData[firstPart] = obj;
  } else {
    containerData[firstPart].AddChild(obj, secondPart);
  }

}
