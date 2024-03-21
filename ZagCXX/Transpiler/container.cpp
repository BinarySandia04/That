#include "container.h"

#include <iostream>

#include <ZagIR/Logs/logs.h>

#include "termcolor/termcolor.hpp"

using namespace ZagIR;
using namespace ZagCXX;

ObjectContainer::ObjectContainer() {}

ObjectContainer::ObjectContainer(std::vector<Binding *> *bindings) {
  for (int i = 0; i < bindings->size(); i++) {
    AddBinding((*bindings)[i]);
  }
}

ObjectContainer::~ObjectContainer() {
  for (auto &p : containerData) {
    delete p.second;
  }
}

void ObjectContainer::Print(int space) {
  std::cout << std::string(space, ' ') << "[ObjectContainer]" << std::endl;
  for (auto &p : containerData) {
    std::cout << std::string(space + 3, ' ') << termcolor::yellow << p.first
              << termcolor::reset << ": ";
    if (p.second != nullptr)
      p.second->Print(space + 3);
    else
      std::cout << "nullptr" << std::endl;
  }
}

Object* ObjectContainer::Clone(){
  ObjectContainer* newContainer = new ObjectContainer();
  for(auto &p : containerData){
    newContainer->AddObject(p.second->Clone(), p.first);
  }
  return newContainer;
}

void ObjectContainer::AddObject(Object *obj, std::string path) {
  // FirstPart: io
  // SecondPart: hola.tal.print
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

  if (secondPart == "") {
    containerData[firstPart] = obj;
  } else {
    ObjectContainer *subContainer;
    if (containerData.find(firstPart) != containerData.end()) {
      // Comprovem que es container
      subContainer = dynamic_cast<ObjectContainer *>(containerData[firstPart]);
      if (subContainer == nullptr) {
        std::cout << firstPart << "|" << secondPart << std::endl;
        Logs::Error("Tried to add object to an existing container");
        return;
      }
    } else {
      subContainer = new ObjectContainer();
      containerData[firstPart] = subContainer;
    }

    subContainer->AddObject(obj, secondPart);
  }
}

void ObjectContainer::AddBinding(ZagIR::Binding *b) {
  AddObject(GetObjectFromBinding(b), b->name);
}

Object *ObjectContainer::GetObject(std::string key) {
  return containerData[key];
}

void ObjectContainer::Merge(ObjectContainer* other){
  for(auto &p : other->containerData){
    AddObject(p.second->Clone(), p.first);
  }
}


