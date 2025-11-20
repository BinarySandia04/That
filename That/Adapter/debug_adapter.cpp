#include "debug_adapter.h"

#include <iostream>

using namespace That;

DebugAdapter::DebugAdapter() {}

DebugAdapter::~DebugAdapter() {}

// Functions to be overriden
void DebugAdapter::OpenBlock() { std::cout << "OpenBlock()" << std::endl; }
void DebugAdapter::CloseBlock() { std::cout << "CloseBlock()" << std::endl; }

void DebugAdapter::Assignation() { std::cout << "Assignation()" << std::endl; }
void DebugAdapter::AssignationVariable(std::string s) {
  std::cout << "AssignationVariable(" << s << ")" << std::endl;
} // Assignem una variable no un acces
void DebugAdapter::NewVariable() {
  std::cout << "NewVariable()" << std::endl;
} // La variable declarada es nova (TODO: es pot posar de millor forma?)

void DebugAdapter::Int(std::string s) { std::cout << "Int(" << s << ")" << std::endl; }
void DebugAdapter::Num(std::string s) { std::cout << "Num(" << s << ")" << std::endl; }
void DebugAdapter::Str(std::string s) { std::cout << "Str(" << s << ")" << std::endl; }
void DebugAdapter::Yep() { std::cout << "Yep()" << std::endl; }
void DebugAdapter::Nop() { std::cout << "Nop()" << std::endl; }
void DebugAdapter::Chr(std::string s) { std::cout << "Chr(" << s << ")" << std::endl; }
void DebugAdapter::Id(std::string s) { std::cout << "Id(" << s << ")" << std::endl; }

void DebugAdapter::Array(int n) { std::cout << "Array(" << n << ")" << std::endl; }
void DebugAdapter::Binary(std::string s) { std::cout << "Binary(" << s << ")" << std::endl; }
void DebugAdapter::OpenUnary(std::string s) { std::cout << "OpenUnary(" << s << std::endl; }
void DebugAdapter::CloseUnary() { std::cout << "CloseUnary()" << std::endl; }

void DebugAdapter::If() { std::cout << "If()" << std::endl; }
void DebugAdapter::Else() { std::cout << "Else()" << std::endl; }
void DebugAdapter::ElseIf() { std::cout << "ElseIf()" << std::endl; }

void DebugAdapter::Forever() { std::cout << "Forever()" << std::endl; }
void DebugAdapter::IntervalCountFor() { std::cout << "IntervalCountFor()" << std::endl; }
void DebugAdapter::IntervalRangeFor() { std::cout << "IntervalRangeFor()" << std::endl; }
void DebugAdapter::For() { std::cout << "For()" << std::endl; }
void DebugAdapter::CountFor() {
  std::cout << "CountFor()" << std::endl;
} // Can be normal while or a repeat

void DebugAdapter::Get(std::string s) { std::cout << "Get(" << s << ")" << std::endl; }
void DebugAdapter::NativeCall(std::string s) {
  std::cout << "NativeCall(" << s << ")" << std::endl;
} // C-call
void DebugAdapter::Call(std::string s) { std::cout << "Call(" << s << ")" << std::endl; }
void DebugAdapter::CallArgs(int n) { std::cout << "CallArgs(" << n << ")" << std::endl; }
void DebugAdapter::ReadArg() { std::cout << "ReadArg()" << std::endl; }

void DebugAdapter::Access() { std::cout << "Access()" << std::endl; }

void DebugAdapter::Function(std::string s, int n) {
  std::cout << "Function(" << s << "," << n << ")" << std::endl;
} // Nom, nºarguments
void DebugAdapter::Argument(std::string s) {
  std::cout << "Argument(" << s << ")" << std::endl;
} // Nom d'argument
void DebugAdapter::Return() { std::cout << "Return()" << std::endl; }

void DebugAdapter::Break() { std::cout << "Break()" << std::endl; }
void DebugAdapter::Break(std::string s) { std::cout << "Break(" << s << ")" << std::endl; }
void DebugAdapter::BreakIf() { std::cout << "BreakIf()" << std::endl; }

void DebugAdapter::LoadPackage(std::string s) {
  std::cout << "LoadPackage(" << s << ")" << std::endl;
}
void DebugAdapter::LoadCode(std::string s) {
  std::cout << "LoadCode(" << s << ")" << std::endl;
}

void DebugAdapter::StartKin(std::string s) {
  std::cout << "StartKin(" << s << ")" << std::endl;
} // kin name
void DebugAdapter::EndKin() { std::cout << "EndKin()" << std::endl; }
void DebugAdapter::SetAttributePrivacy(Privacy p) {
  std::cout << "SetAttributePrivacy(" << p << ")" << std::endl;
}
void DebugAdapter::AttributeAssignation(std::string s) {
  std::cout << "AttributeAssignation(" << s << ")" << std::endl;
} // Name
void DebugAdapter::AttributeFunction(std::string s) {
  std::cout << "AttributeFunction(" << s << ")" << std::endl;
} // Function name (???)
void DebugAdapter::AttributeIdentifier(std::string s) {
  std::cout << "AttributeIdentifier(" << s << ")" << std::endl;
} // Identifier name

