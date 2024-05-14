#pragma once

#include <string>
#include <iostream>

#include "adapter.h"
#include "Objects/object.h"

namespace ThatLib {

class DebugAdapter : public Adapter {
public:
  DebugAdapter();
  ~DebugAdapter();

  // Functions to be overriden
  void OpenBlock();
  void CloseBlock();
  void Assignation();
  void AssignationVariable(std::string s);
  void NewVariable();
  void Int(std::string s);
  void Num(std::string s);
  void Str(std::string s);
  void Yep();
  void Nop();
  void Chr(std::string s);
  void Id(std::string s);
  void Array(int n);
  void Binary(std::string s);
  void OpenUnary(std::string s);
  void CloseUnary();
  void If();
  void Else();
  void ElseIf();
  void Forever();
  void IntervalCountFor();
  void IntervalRangeFor();
  void For();
  void CountFor();
  void Get(std::string s);
  void NativeCall(std::string s);
  void Call(std::string s);
  void CallArgs(int n);
  void ReadArg();
  void Access();
  void Function(std::string s, int n);
  void Argument(std::string s);
  void Return();
  void Break();
  void Break(std::string s);
  void BreakIf();
  void LoadPackage(std::string s);
  void LoadCode(std::string s);
  void StartKin(std::string s);
  void EndKin();
  void SetAttributePrivacy(Privacy p);
  void AttributeAssignation(std::string s);
  void AttributeFunction(std::string s);
  void AttributeIdentifier(std::string s);

};

}; // namespace ThatLib
