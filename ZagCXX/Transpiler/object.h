#pragma once

#include <string>
#include <unordered_map>

#include <ZagIR/Libs/packages.h>

#include "environment.h"

using namespace ZagIR;

namespace ZagCXX {

class Object;
class Usable;
class ObjectType;
class ObjectEmpty;
class ObjectVariable;
class ObjectContainer;
class ObjectFunction;
class ObjectCFunction;
class ObjectCOperation;
class ObjectProtoType;
class ObjectType;
class Environment;

Object *GetObjectFromBinding(Binding *);

class Object {
public:
  Object(){};
  virtual ~Object(){};
  virtual void Print(int);

  virtual Object* Clone() = 0;

  std::string identifier; // Hauriem de posar const XD
};

class Usable {
public:
  virtual void Use(Environment *){};
};

class ObjectEmpty : public Object {
public:
  void Print(int);
  Object* Clone();
};

class ObjectVariable : public Object {
public:
  void Print(int);
  Object* Clone();

  ObjectVariable(ObjectType *, std::string name);

  void SetType(ObjectType *);
  ObjectType *GetType();

  std::string Transpile();

private:
  std::string name;
  ObjectType *type;
};

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

class ObjectFunction : public Object {
public:
  void Print(int);
  Object* Clone();

  void SetInheritedType(ObjectType *);

  virtual std::string GetName();

  bool CheckArgs(std::vector<ObjectType *> &, Environment *env);

  std::vector<std::string> functionArgs;
  std::string returnType;
private:
  ObjectType *inheritedType;
};

class ObjectCFunction : public ObjectFunction, public Usable {
public:
  std::string GetName();
  ObjectCFunction(CFunction *);
  void Print(int);
  Object* Clone();

  void Use(Environment *);

private:
  CFunction *cFunctionData;
};

class ObjectNativeFunction : public ObjectFunction {
public:
  std::string GetName();
  void Print(int);
  Object* Clone();
};

class ObjectConversion : public Object {
public:
  ObjectConversion(Conversion *);
  void Print(int);
  Object* Clone();

private:
  Conversion *conversion;
};

// Objecte que crea classes
class ObjectProtoType : public Object, public Usable {
public:
  ObjectProtoType(CType *);
  ~ObjectProtoType();

  void Print(int);
  Object* Clone();
  void Use(Environment *);
  ObjectType *Construct(std::vector<ObjectType *>, Environment *);

  ObjectContainer *typeMethods;

private:
  // Com que els tipus de classe són purament C, els abstraim
  // des d'aqui
  CType *cTypeInfo;
};

class ObjectType : public Object {
public:
  void Print(int);
  Object* Clone();
  bool Equals(ObjectType *);
  bool AbstractedFrom(ObjectType *);

  std::string identifier;
  std::string translation;
  std::string upgrades_to;

  // ObjectType *parent;
  std::vector<ObjectType *> children;

  std::string Transpile();

  ObjectProtoType* constructor;
private:

  friend class ObjectProtoType;
};

class ObjectCOperation : public Object, public Usable {
public:
  std::string GetName();
  ObjectCOperation(COperation *);
  void Print(int);
  Object* Clone();
  void Use(Environment *);
  COperation *cOperationData;
};

} // namespace ZagCXX
