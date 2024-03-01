#pragma once

#include <string>
#include <unordered_map>

#include <ZagIR/Libs/packages.h>

namespace ZagCXX {
class Object {
public:
  virtual ~Object();
  virtual void Print();
};

class ObjectEmpty : private Object {
public:
  void Print();
};

class ObjectVariable : private Object {
public:
  void Print();

  ObjectVariable(std::string);
  std::string varName;
};

class ObjectContainer : private Object {
public:
  void Print();

  void AddObject(Object *, std::string);
  Object *GetObject(std::string);

private:
  std::unordered_map<std::string, Object *> containerData;
};

class ObjectFunction : private Object {
  void Print();
};

class ObjectCFunction : private Object {
public:
  void Print();

private:
  ZagIR::CFunction cFunctionData;
  std::vector<ObjectVariable> functionArgs;
  ObjectVariable *functionReturn;
};

class ObjectConversion : private Object {
public:
  void Print();
};

class ObjectType : private Object {
public:
  void Print();

  std::string name;
  std::vector<ObjectVariable *> children;

  virtual std::string Transpile();

private:
  virtual std::string TranspileChildren();
};

class ObjectCType : private ObjectType {
public:
  void Print();

  bool internal;
  std::string translation;
  std::string includes;

  std::string Transpile();

private:
  std::string TranspileChildren();
};

class ObjectNativeType : private ObjectType {
  std::string Transpile();

private:
  std::string TranspileChildren();
};
} // namespace ZagCXX
