#pragma once

#include "object.h"

using namespace ZagIR;

namespace ZagCXX {

class Object;

enum Privacy {
  PUBLIC,
  PRIVATE,
  STATIC,
};

class ObjectContainerData {
public:
  ObjectContainerData(Privacy, Object*);
  ~ObjectContainerData();
  ObjectContainerData* Clone();
  Privacy privacy;
  Object* object;
};

class ObjectContainer : public Object {
public:
  ObjectContainer();
  ObjectContainer(std::vector<Binding*>*);
  ~ObjectContainer();

  void Print(int);
  Object* Clone();

  void AddObject(Object *, std::string);
  void AddObject(Object *, std::string, Privacy);

  void AddBinding(Binding *);
  Object *GetObject(std::string);
  void Merge(ObjectContainer *);

private:
  std::unordered_map<std::string, ObjectContainerData *> containerData;
};

};
