#pragma once

#include <string>
#include <unordered_map>

#include <ZagIR/Libs/packages.h>

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
class ObjectNativeType;

Object* GetObjectFromBinding(ZagIR::Binding *);

class Object {
public:
  Object() {};
  Object(ZagIR::Binding *);
  virtual ~Object() {};
  virtual void Print(int);
  std::string identifier; // Hauriem de posar const XD
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
  void Print(int);

private:
  ZagIR::CFunction* cFunctionData;
};

class ObjectNativeFunction : public ObjectFunction {
public:
  std::string GetName();
  void Print(int);
  void SetReturnType(ObjectType* );
};

class ObjectConversion : public Object {
public:
  ObjectConversion(ZagIR::Conversion *);
  void Print(int);
private:
  ZagIR::Conversion *conversion;
};

class ObjectType : public Object {
public:
  void Print(int);
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
  bool Equals(ObjectType *);

  bool internal;
  std::string translation;
  std::vector<std::string> includes;

  std::string Transpile();
};

class ObjectNativeType : public ObjectType {
  bool Equals(ObjectType *);
  void Print(int);
  std::string Transpile();
};
} // namespace ZagCXX
