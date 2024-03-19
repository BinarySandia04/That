#pragma once

#include <string>
#include <unordered_map>

#include <ZagIR/Libs/packages.h>

#include "environment.h"

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

Object *GetObjectFromBinding(ZagIR::Binding *);

class Object {
public:
  Object(){};
  virtual ~Object(){};
  virtual void Print(int);
  std::string identifier; // Hauriem de posar const XD
};

class Usable {
public:
  virtual void Use(Environment *){};
};

class ObjectEmpty : public Object {
public:
  void Print(int);
};

class ObjectVariable : public Object {
public:
  void Print(int);
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
  ~ObjectContainer();

  void Print(int);

  void AddObject(Object *, std::string);
  void AddBinding(ZagIR::Binding *);
  Object *GetObject(std::string);

private:
  std::unordered_map<std::string, Object *> containerData;
};

class ObjectFunction : public Object {
public:
  void Print(int);
  virtual std::string GetName();

  bool CheckArgs(std::vector<ObjectType *> &, Environment *env);

  std::vector<std::string> functionArgs;
  std::string returnType;
};

class ObjectCFunction : public ObjectFunction, public Usable {
public:
  std::string GetName();
  ObjectCFunction(ZagIR::CFunction *);
  void Print(int);
  void Use(Environment *);

private:
  ZagIR::CFunction *cFunctionData;
};

class ObjectNativeFunction : public ObjectFunction {
public:
  std::string GetName();
  void Print(int);
};

class ObjectConversion : public Object {
public:
  ObjectConversion(ZagIR::Conversion *);
  void Print(int);

private:
  ZagIR::Conversion *conversion;
};

// Objecte que crea classes
class ObjectProtoType : public Object, public Usable {
public:
  ObjectProtoType(ZagIR::CType *);
  void Print(int);
  void Use(Environment *);
  ObjectType *Construct(std::vector<ObjectType *>, Environment *);

private:
  // Com que els tipus de classe són purament C, els abstraim
  // des d'aqui
  ZagIR::CType *cTypeInfo;
};

class ObjectType : public Object {
public:
  void Print(int);
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
  ObjectCOperation(ZagIR::COperation *);
  void Print(int);
  void Use(Environment *);
  ZagIR::COperation *cOperationData;
};

} // namespace ZagCXX
