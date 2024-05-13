#pragma once

#include <ThatLib/Transpiler/adapter.h>

#include <string>

namespace ThatCXX {

class CppAdapter : public ThatLib::Adapter {
public:
  CppAdapter();
  std::string GetResult();

  // --- Override methods
  /*
  void OpenBlock();
  void CloseBlock();

  void Int(std::string data);
  void Num(std::string data);
  void Str(std::string data);
  void Yep();
  void Nop();
  void Chr(std::string data);
  void Id(std::string data);
  void Binary(std::string op);
  */

private:
  std::string res;
};

}; // namespace ThatCXX
