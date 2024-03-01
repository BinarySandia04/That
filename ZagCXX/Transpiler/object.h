#pragma once

#include <string>
#include <unordered_map>

#include <ZagIR/Libs/packages.h>

namespace ZagCXX {
class Object {
public:
  virtual ~Object();
  void Print();

  ZagIR::Package *GetPackage();
private:
  ZagIR::Package *package;
};

class ObjectEmpty : private Object {};

class ObjectVariable : private Object {
  std::string varName;
};

class ObjectContainer : private Object {
  public:
    void AddObject(Object*, std::string);
    Object *GetObject(std::string);
  private:
    std::unordered_map<std::string, Object> containerData;
};

class ObjectFunction : private Object {

};

class ObjectCFunction : private Object {
  ZagIR::FunctionCall cFunctionData;
  std::vector<ObjectVariable> functionArgs;
  ObjectVariable* functionReturn;
};

class ObjectConversion : private Object {

};


class ObjectType : private Object {
  public:
    std::string name;
    std::vector<ObjectVariable *> children;

    virtual std::string Transpile();
  private:
    virtual std::string TranspileChildren();
};

class ObjectCType : private ObjectType {
    bool internal;
    std::string translation;
    std::string includes;

    std::string Transpile();
  private:
    std::string TranspileChildren();
}
};

class ObjectNativeType : private ObjectType {
    std::string Transpile();
  private:
    std::string TranspileChildren();
};
} // namespace ZagCXX
