#pragma once

#include <string>
#include <unordered_map>

#include <ThatIR/Libs/packages.h>


using namespace ThatIR;

namespace ThatCXX {

class Object {
public:
  Object(){};
  virtual ~Object(){};
  virtual void Print(int);

  virtual Object* Clone() = 0;

  std::string identifier; // Hauriem de posar const XD
};

class ObjectEmpty : public Object {
public:
  void Print(int);
  Object* Clone();
};

Object *GetObjectFromBinding(Binding *);

} // namespace ThatCXX
