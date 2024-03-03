#pragma once

#include <string>
#include <unordered_map>

#include <ZagIR/Libs/packages.h>

namespace ZagCXX {
class Object {
public:
  virtual ~Object() {};
  virtual void Print();
  std::string identifier; // Hauriem de posar const XD
};

class ObjectType;

class ObjectEmpty : public Object {
public:
  void Print();
};

class ObjectVariable : public Object {
public:
  void Print();
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
  void Print();

  void AddObject(Object *, std::string);
  Object *GetObject(std::string);

private:
  std::unordered_map<std::string, Object *> containerData;
};

class ObjectFunction : public Object {
public:
  void Print();
  virtual std::string GetName();

  ObjectType* GetReturnType();
  bool CheckArgs(std::vector<ObjectType *>&);
  std::vector<ObjectType *> functionArgs;
protected:
  ObjectType *functionReturn;
};

class ObjectCFunction : public ObjectFunction {
public:
  std::string GetName();
  ObjectCFunction(ZagIR::CFunction *);
  void Print();

private:
  ZagIR::CFunction* cFunctionData;
};

class ObjectNativeFunction : public ObjectFunction {
public:
  std::string GetName();
  void Print();
  void SetReturnType(ObjectType* );
};

class ObjectConversion : public Object {
public:
  ObjectConversion(ZagIR::Conversion *);
  void Print();
private:
  ZagIR::Conversion *conversion;
};

class ObjectType : public Object {
public:
  void Print();
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
  void Print();
  bool Equals(ObjectType *);

  bool internal;
  std::string translation;
  std::vector<std::string> includes;

  std::string Transpile();
};

class ObjectNativeType : public ObjectType {
  bool Equals(ObjectType *);
  void Print();
  std::string Transpile();
};
} // namespace ZagCXX
