#pragma once

#include "object.h"

using namespace ZagIR;

namespace ZagCXX {

class Object;

class ObjectContainer : public Object {
public:
  ObjectContainer();
  ObjectContainer(std::vector<Binding*>*);
  ~ObjectContainer();

  void Print(int);
  Object* Clone();

  void AddObject(Object *, std::string);
  void AddBinding(Binding *);
  Object *GetObject(std::string);
  void Merge(ObjectContainer *);

private:
  std::unordered_map<std::string, Object *> containerData;
};

};
