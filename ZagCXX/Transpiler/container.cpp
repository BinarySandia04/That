#include "container.h"

#include <iostream>

#include <ZagIR/Logs/logs.h>

#include "termcolor/termcolor.hpp"

using namespace ZagIR;
using namespace ZagCXX;

ObjectContainerData::ObjectContainerData(Privacy p, Object* obj){
  this->privacy = p;
  this->object = obj;
}

ObjectContainerData::~ObjectContainerData(){
  delete object;
}

ObjectContainerData* ObjectContainerData::Clone(){
  return new ObjectContainerData(privacy, object->Clone());
}

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
      p.second->object->Print(space + 3);
    else
      std::cout << "nullptr" << std::endl;
  }
}

Object* ObjectContainer::Clone(){
  ObjectContainer* newContainer = new ObjectContainer();
  for(auto &p : containerData){
    ObjectContainerData* clonedData = p.second->Clone();
    newContainer->AddObject(clonedData->object, p.first, clonedData->privacy);
    delete clonedData;
  }
  return newContainer;
}

void ObjectContainer::AddObject(Object *obj, std::string path){
  AddObject(obj, path, PUBLIC);
}

void ObjectContainer::AddObject(Object *obj, std::string path, Privacy p) {
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
    containerData[firstPart] = new ObjectContainerData(p, obj);
  } else {
    ObjectContainer *subContainer;
    if (containerData.find(firstPart) != containerData.end()) {
      // Comprovem que es container
      subContainer = dynamic_cast<ObjectContainer *>(containerData[firstPart]->object);
      if (subContainer == nullptr) {
        std::cout << firstPart << "|" << secondPart << std::endl;
        Logs::Error("Tried to add object to an existing container");
        return;
      }
    } else {
      subContainer = new ObjectContainer();
      containerData[firstPart] = new ObjectContainerData(p, subContainer);
    }

    subContainer->AddObject(obj, secondPart);
  }
}

void ObjectContainer::AddBinding(ZagIR::Binding *b) {
  AddObject(GetObjectFromBinding(b), b->name);
}

Object *ObjectContainer::GetObject(std::string key) {
  return containerData[key]->object;
}

void ObjectContainer::Merge(ObjectContainer* other){
  for(auto &p : other->containerData){
    ObjectContainerData* dupedData = p.second->Clone();
    AddObject(dupedData->object, p.first, dupedData->privacy);
    delete dupedData;
  }
}


