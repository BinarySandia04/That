#pragma once

#include "object.h"

using namespace ThatIR;

namespace ThatCXX {

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
  ObjectContainer(ObjectContainer*);
  ObjectContainer(std::vector<Binding*>*);
  ~ObjectContainer();

  void Print(int);
  Object* Clone();

  void AddObject(std::string, Object*);
  void AddObject(std::string, Object*, Privacy);

  void AddBinding(Binding *);
  Object* GetObject(std::string);
  Object* Fetch(std::string);
  void Merge(ObjectContainer *);

  bool Exists(std::string);

  auto begin() const { return containerData.begin(); }
  auto end() const { return containerData.end(); }

  ObjectContainer* before;

private:
  std::unordered_map<std::string, ObjectContainerData *> containerData;
};

};
