#pragma once

#include <string>

namespace ThatLib {

class Adapter {
public:
  Adapter();
  ~Adapter();

  // Functions to be overriden
  virtual void OpenBlock();
  virtual void CloseBlock();

  virtual void Int(std::string);
  virtual void Num(std::string);
  virtual void Str(std::string);
  virtual void Yep();
  virtual void Nop();
  virtual void Chr(std::string);
  virtual void Id(std::string);
  virtual void Binary(std::string);

private:
  // Private
};

}; // namespace ThatLib
