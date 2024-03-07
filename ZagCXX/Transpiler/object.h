#pragma once

#include <string>
#include <unordered_map>

#include <ZagIR/Libs/packages.h>

#include "environment.h"

namespace ZagCXX {

class Object;
class ObjectType;
class ObjectEmpty;
class ObjectVariable;
class ObjectContainer;
class ObjectFunction;
class ObjectCFunction;
class ObjectType;
class ObjectCType;
class Environment;
class ObjectNativeType;

Object *GetObjectFromBinding(ZagIR::Binding *);

class Object {
public:
  Object(){};
  virtual ~Object(){};
  virtual void Print(int);
  virtual void Use(Environment*);
  std::string identifier; // Hauriem de posar const XD
};

class ObjectEmpty : public Object {
public:
  void Print(int);
  void Use(Environment*);
};

class ObjectVariable : public Object {
public:
  void Print(int);
  void Use(Environment*);
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
  void Use(Environment*);

  void AddObject(Object *, std::string);
  void AddBinding(ZagIR::Binding *);
  Object *GetObject(std::string);

private:
  std::unordered_map<std::string, Object *> containerData;
};

class ObjectFunction : public Object {
public:
  void Print(int);
  void Use(Environment*);
  virtual std::string GetName();

  bool CheckArgs(std::vector<ObjectType *> &);

  std::vector<std::string> functionArgs;
  std::string returnType;
};

class ObjectCFunction : public ObjectFunction {
public:
  std::string GetName();
  ObjectCFunction(ZagIR::CFunction *);
  void Print(int);
  void Use(Environment*);

private:
  ZagIR::CFunction *cFunctionData;
};

class ObjectNativeFunction : public ObjectFunction {
public:
  std::string GetName();
  void Print(int);
  void Use(Environment*);
};

class ObjectConversion : public Object {
public:
  ObjectConversion(ZagIR::Conversion *);
  void Print(int);
  void Use(Environment*);

private:
  ZagIR::Conversion *conversion;
};

class ObjectType : public Object {
public:
  void Print(int);
  void Use(Environment*);
  virtual bool Equals(ObjectType *);

  std::string upgrades_to;
  std::vector<ObjectVariable *> children;

  virtual std::string Transpile();

private:
  std::string TranspileChildren();
};

class ObjectCType : public ObjectType {
public:
  ObjectCType(ZagIR::CType *);
  void Print(int);
  void Use(Environment*);
  bool Equals(ObjectType *);

  bool internal;
  std::string translation;
  std::vector<std::string> includes;

  std::string Transpile();
};

class ObjectNativeType : public ObjectType {
  bool Equals(ObjectType *);
  void Print(int);
  void Use(Environment*);
  std::string Transpile();
};
} // namespace ZagCXX
