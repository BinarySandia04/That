#pragma once

#include <iostream>
#include <string>

#include "Objects/object.h"

namespace ThatLib {

class Adapter {
public:
  Adapter();
  ~Adapter();

  // Functions to be overriden
  virtual void OpenBlock() {}
  virtual void CloseBlock() {}

  virtual void Assignation() {}
  virtual void AssignationVariable(std::string s) {}
  virtual void NewVariable() {}
  virtual void Int(std::string s) {}
  virtual void Num(std::string s) {}
  virtual void Str(std::string s) {}
  virtual void Yep() {}
  virtual void Nop() {}
  virtual void Chr(std::string s) {}
  virtual void Id(std::string s) {}

  virtual void Array(int n) {}

  virtual void Binary(std::string s) {}
  virtual void OpenUnary(std::string s) {}
  virtual void CloseUnary() {}

  virtual void If() {}
  virtual void Else() {}
  virtual void ElseIf() {}

  virtual void Forever() {}
  virtual void IntervalCountFor() {}
  virtual void IntervalRangeFor() {}
  virtual void For() {}
  virtual void CountFor() {} // Can be normal while or a repeat

  virtual void Get(std::string s) {}
  virtual void NativeCall(std::string s) {} // C-call
  virtual void Call(std::string s) {}
  virtual void CallArgs(int n) {}
  virtual void ReadArg() {}

  virtual void Access() {}

  virtual void Function(std::string s, int n) {} // Nom, nºarguments
  virtual void Argument(std::string s) {}        // Nom d'argument
  virtual void Return() {}

  virtual void Break() {}
  virtual void Break(std::string s) {}
  virtual void BreakIf() {}

  virtual void LoadPackage(std::string s) {}
  virtual void LoadCode(std::string s) {}

  virtual void StartKin(std::string s) {} // kin name
  virtual void EndKin() {}
  virtual void SetAttributePrivacy(Privacy p) {}
  virtual void AttributeAssignation(std::string s) {} // Name
  virtual void AttributeFunction(std::string s) {}    // Function name (???)
  virtual void AttributeIdentifier(std::string s) {}  // Identifier name

private:
  // Private
};

}; // namespace ThatLib
