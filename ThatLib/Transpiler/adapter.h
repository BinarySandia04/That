#pragma once

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

  virtual void OpenAssignation() {}
  virtual void AssignationVariable(std::string) {
  } // Assignem una variable no un acces
  virtual void AssignationNewVariable() {
  } // La variable declarada es nova (TODO: es pot posar de millor forma?)
  virtual void AssignationValue(); // La assignació té un valor
  virtual void CloseAssignation() {}

  virtual void Int(std::string) {}
  virtual void Num(std::string) {}
  virtual void Str(std::string) {}
  virtual void Yep() {}
  virtual void Nop() {}
  virtual void Chr(std::string) {}
  virtual void Id(std::string) {}

  virtual void Array(int) {}

  virtual void OpenBinary(std::string) {}
  virtual void CloseBinary() {}
  virtual void OpenUnary(std::string) {}
  virtual void CloseUnary() {}

  virtual void If() {}
  virtual void Else() {}
  virtual void ElseIf() {}

  virtual void Forever() {}
  virtual void IntervalCountFor() {}
  virtual void IntervalRangeFor() {}
  virtual void For() {}
  virtual void CountFor() {} // Can be normal while or a repeat

  virtual void Function(std::string, int) {} // Nom, nºarguments
  virtual void Argument(std::string) {}      // Nom d'argument
  virtual void Return() {}

  virtual void Break() {}
  virtual void Break(std::string) {}
  virtual void BreakIf() {}

  virtual void LoadPackage(std::string) {}
  virtual void LoadCode(std::string) {}

  virtual void StartKin(std::string) {} // kin name
  virtual void EndKin() {}
  virtual void SetAttributePrivacy(Privacy) {}
  virtual void AttributeAssignation(std::string) {} // Name
  virtual void AttributeFunction(std::string) {}    // Function name (???)
  virtual void AttributeIdentifier(std::string) {}  // Identifier name

private:
  // Private
};

}; // namespace ThatLib
