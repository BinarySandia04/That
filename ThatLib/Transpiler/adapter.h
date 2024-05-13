#pragma once

#include <string>
#include <iostream>

#include "Objects/object.h"

namespace ThatLib {

class Adapter {
public:
  Adapter();
  ~Adapter();

  // Functions to be overriden
  virtual void OpenBlock() {
    std::cout << "OpenBlock()" << std::endl;
  }
  virtual void CloseBlock() {
    std::cout << "CloseBlock()" << std::endl;
  }

  virtual void OpenAssignation() {
    std::cout << "OpenAssignation()" << std::endl;
  }
  virtual void AssignationVariable(std::string s) {
    std::cout << "AssignationVariable(" << s << ")" << std::endl;
  } // Assignem una variable no un acces
  virtual void AssignationNewVariable() {
    std::cout << "AssignationNewVariable()" << std::endl;
  } // La variable declarada es nova (TODO: es pot posar de millor forma?)
  virtual void AssignationValue() {
    std::cout << "AssignationValue()" << std::endl;
  } // La assignació té un valor
  virtual void CloseAssignation() {
    std::cout << "CloseAssignation()" << std::endl;
  }

  virtual void Int(std::string s) {
    std::cout << "Int(" << s << ")" << std::endl;
  }
  virtual void Num(std::string s) {
    std::cout << "Num(" << s << ")" << std::endl;
  }
  virtual void Str(std::string s) {
    std::cout << "Str(" << s << ")" << std::endl;
  }
  virtual void Yep() {
    std::cout << "Yep()" << std::endl;
  }
  virtual void Nop() {
    std::cout << "Nop()" << std::endl;
  }
  virtual void Chr(std::string s) {
    std::cout << "Chr(" << s << ")" << std::endl;
  }
  virtual void Id(std::string s) {
    std::cout << "Id(" << s << ")" << std::endl;
  }

  virtual void Array(int n) {
    std::cout << "Array(" << n << ")" << std::endl;
  }

  virtual void Binary(std::string s) {
    std::cout << "Binary(" << s << ")" << std::endl; 
  }
  virtual void OpenUnary(std::string s) {
    std::cout << "OpenUnary(" << s << std::endl;
  }
  virtual void CloseUnary() {
    std::cout << "CloseUnary()" << std::endl;
  }

  virtual void If() {
    std::cout << "If()" << std::endl;
  }
  virtual void Else() {
    std::cout << "Else()" << std::endl;
  }
  virtual void ElseIf() {
    std::cout << "ElseIf()" << std::endl;
  }

  virtual void Forever() {
    std::cout << "Forever()" << std::endl;
  }
  virtual void IntervalCountFor() {
    std::cout << "IntervalCountFor()" << std::endl;
  }
  virtual void IntervalRangeFor() {
    std::cout << "IntervalRangeFor()" << std::endl;
  }
  virtual void For() {
    std::cout << "For()" << std::endl;
  }
  virtual void CountFor() {
    std::cout << "CountFor()" << std::endl;
  } // Can be normal while or a repeat

  virtual void Get(std::string s) {
    std::cout << "Get(" << s << ")" << std::endl;
  }
  virtual void NativeCall(std::string s) {
    std::cout << "NativeCall(" << s << ")" << std::endl;
  } // C-call
  virtual void Call(std::string s) {
    std::cout << "Call(" << s << ")" << std::endl;
  }
  virtual void CallArgs(int n) {
    std::cout << "CallArgs(" << n << ")" << std::endl;
  }
  virtual void ReadArg() {
    std::cout << "ReadArg()" << std::endl;
  } 

  virtual void Access() {
    std::cout << "Access()" << std::endl;
  }

  virtual void Function(std::string s, int n) {
    std::cout << "Function(" << s << "," << n << ")" << std::endl;
  } // Nom, nºarguments
  virtual void Argument(std::string s) {
    std::cout << "Argument(" << s << ")" << std::endl;
  }      // Nom d'argument
  virtual void Return() {
    std::cout << "Return()" << std::endl;
  }

  virtual void Break() {
    std::cout << "Break()" << std::endl;
  }
  virtual void Break(std::string s) {
    std::cout << "Break(" << s << ")" << std::endl;
  }
  virtual void BreakIf() {
    std::cout << "BreakIf()" << std::endl;
  }

  virtual void LoadPackage(std::string s) {
    std::cout << "LoadPackage(" << s << ")" << std::endl;
  }
  virtual void LoadCode(std::string s) {
    std::cout << "LoadCode(" << s << ")" << std::endl;
  }

  virtual void StartKin(std::string s) {
    std::cout << "StartKin(" << s << ")" << std::endl;
  } // kin name
  virtual void EndKin() {
    std::cout << "EndKin()" << std::endl;
  }
  virtual void SetAttributePrivacy(Privacy p) {
    std::cout << "SetAttributePrivacy(" << p << ")" << std::endl;
  }
  virtual void AttributeAssignation(std::string s) {
    std::cout << "AttributeAssignation(" << s << ")" << std::endl;
  } // Name
  virtual void AttributeFunction(std::string s) {
    std::cout << "AttributeFunction(" << s << ")" << std::endl;
  }    // Function name (???)
  virtual void AttributeIdentifier(std::string s) {
    std::cout << "AttributeIdentifier(" << s << ")" << std::endl;
  }  // Identifier name

private:
  // Private
};

}; // namespace ThatLib
